### Cryptopp使用记录

#### `SecBlock`

主要使用的是`SecByteBlock`，定义如下

```c++
typedef SecBlock<byte, AllocatorWithCleanup<byte, false> > SecByteBlock;
```

- `SecByteBlock`的单位是`byte`（Cryptopp中自定义类型，实质是`char`）

- `AllocatorWithCleanup`是内存分配器，内存分配的单位是`byte`

  ```c++
  /// T: class or type
  /// T_Align16: boolean that determines whether allocations should be aligned on a 16-byte boundary
  template <class T, bool T_Align16 = false>
  class AllocatorWithCleanup : public AllocatorBase<T>
  {
      /* ... */
  }
  ```

  可见`SecByteBlock`的内存分配未开启16字节对齐

**误用说明**：

编写代码时，关于`SecByteBlock`主要有几个误区：

1. 误认为`SecByteBlock`是16字节对齐

   上面可以看出并未开启对齐

2. 误认为对齐指“块大小”按照16字节对齐，即16上取整

   实际上，16字节对齐指内存分配时，地址按照16字节对齐，查阅`AllocatorWithCleanup`可以看出，

   ```c++
   // AllocatorWithCleanup::allocate@cryptopp/secblock.h:206
   pointer allocate(size_type size, const void *ptr = NULLPTR)
   {
       CRYPTOPP_UNUSED(ptr); CRYPTOPP_ASSERT(ptr == NULLPTR);
       this->CheckSize(size);
       if (size == 0)
           return NULLPTR;
   
   #if CRYPTOPP_BOOL_ALIGN16
       if (T_Align16)
           return reinterpret_cast<pointer>(AlignedAllocate(size*sizeof(T)));
   #endif
   
       return reinterpret_cast<pointer>(UnalignedAllocate(size*sizeof(T)));
   }
   
   // AlignedAllocate@cryptopp/allocate.cpp:43
   void * AlignedAllocate(size_t size)
   {
   	byte *p;
   #if defined(CRYPTOPP_MM_MALLOC_AVAILABLE)
   	while ((p = (byte *)_mm_malloc(size, 16)) == NULLPTR)
   #elif defined(CRYPTOPP_MEMALIGN_AVAILABLE)
   	while ((p = (byte *)memalign(16, size)) == NULLPTR)
   /* ... */
   #endif
   ```
   
   `memalign(16, size)`可以看出是16字节对齐地使用malloc分配内存

所以使用`SecByteBlock`时对于长度的计算过于谨慎，没有必要！



####  Crypto ++ `Pipeline`

1. 字节流的异或运算

   ```c++
   // 定义
   // StringSource::StringSource@cryptopp/filters.h:1478
   StringSource(const byte *string, size_t length, bool pumpAll, BufferedTransformation *attachment = NULLPTR) : SourceTemplate<StringStore>(attachment);
   
   // ArraySource本质是StringSource
   ArraySource((byte *)msg[i]->data(), msg[i]->size(), true, new ArrayXorSink(m_register, m_register.size()));
   ```

   - Source：将`pumpAll`设置为`true`，直接构造匿名实例，这样构造时可以直接将`string`传入Source连接的`attachment`，这条语句后便可直接在Sink处拿取结果

   - Sink：这里使用`ArrayXorSink`，是一种byte string做异或运算的处理方式

     `input --> ArrayXorSink(res)`：`res := res`$\oplus$`input`

2. 输入输出的编码解码

   Crypto++过滤器使用类似**修饰者**的设计模式，可以放在Source和Sink的管道流之间，对中间结果进行处理。

   其中`HexEncoder`和`HexDecoder`过滤器实现byte string和hex encoded string之间的互相转换，具体使用可以参考 [Pipeline](https://www.cryptopp.com/wiki/Pipelining)，非常详细

---

### CBC & CTR

1. CBC注意PKCS padding的处理（分为正反向，即加密和解密）

2. CTR注意如何实现IV字节流+1

   直接使用Crypto++提供的库函数`IncrementCounterByOne`

   ```c++
   // increment IV by one
   IncrementCounterByOne(iv_register, iv_register.size());
   ```

   实现值得借鉴

   ```c++
   // Performs an addition with carry by adding 1 on a block of bytes starting at the least
   // significant byte. Once carry is 0, the function terminates and returns to the caller.
   inline void IncrementCounterByOne(byte *inout, unsigned int size)
   {
   	CRYPTOPP_ASSERT(inout != NULLPTR);
   
   	unsigned int carry=1;
   	while (carry && size != 0)
   	{
   		// On carry inout[n] equals 0
   		carry = ! ++inout[size-1];
   		size--;
   	}
   }
   ```

   



