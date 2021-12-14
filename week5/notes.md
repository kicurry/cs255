### C++ specialization

List some examples,

1. Specialize function template

   ```c++
   // function template
   template <typename T>
   int compare(const T &lt, const T &rt)
   {
       if (lt < rt)
       {
           return -1;
       }
       if (rt < lt)
       {
           return 1;
       }
       return 0;
   }
   // specialize for 'char* const &'
   template <>
   int compare(const char *const &lt, const char *const &rt)
   {
       return strcmp(lt, rt);
   }
   ```

2. Specialize class template

   see the next section.

### Hash function for mpz_class

Just stick a part of code, please see header file `hash_mpz.h` for completeness.

```c++
// Specialize std::hash<mpz_srcptr>
template <>
struct std::hash<mpz_srcptr>
{
    size_t operator()(const mpz_srcptr x) const;
};

// Define member functions outside the class
size_t std::hash<mpz_srcptr>::operator()(const mpz_srcptr x) const
{
    uint32_t seed = static_cast<uint32_t>(984124);
    uint32_t res;
    MurmurHash3_x86_32(x->_mp_d, x->_mp_size * sizeof(mp_limb_t), seed, &res);
    return res;
}
```

1. GMP Integer Internal

   - `_mp_d`: a pointer to raw data, array of `mp_limb_t` type.
   - `mp_limb_t`: type of some part of raw data.
   -  `_mp_size`: size of `mp_limb_t` array which indicated by `_mp_d`.

   See more details in [documentation](https://gmplib.org/manual/Integer-Internals).

2. Hash function

   Use **MurmurHash3** as hash function whose source code is downloaded in [github](https://github.com/aappleby/smhasher/tree/master/src).

   Digest raw data which indicated by pointer `_mp_d` to `size_t` value.



---

### Mistakes in PA

1. The shift operator has lower precedence than the arithmetic operator

   ```c++
   ULL x = (ULL)x0 << 20 + x1;   // false, equivalent to '(ULL)x0 << (20 + x1)'
   ULL x = ((ULL)x0 << 20) + x1; // correct
   ```

2. $2^{20}$

   ```c++
   2 << 20 // false
   1 << 20 // correct
   ```

