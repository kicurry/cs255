### SHA256

To use the sha256 hash function, we should include header file `"sha.h"`

see [SHA2](https://www.cryptopp.com/wiki/SHA2) for more detials.



### Misunderstanding

block size means "message size" but not "message size + tag size". In other words, when the browser wants to download one 1KB size block, it download (msg, tag) pair which is actually 1056 bytes(1KB + 32 bytes).



### Mistake

Be careful when using `size_t` type counter decreasingly.

```c++
// False:
for (size_t i = content.size() / BLOCKSIZE * BLOCKSIZE; i >= 0; i -= BLOCKSIZE)
{
    string blk = content.substr(i, BLOCKSIZE) + H;
    H.clear();
    StringSource(blk, true, new HashFilter(hash, new StringSink(H)));
}

// True:
for (int i = content.size() / BLOCKSIZE * BLOCKSIZE; i >= 0; i -= BLOCKSIZE)
{
    string blk = content.substr(i, BLOCKSIZE) + H;
    H.clear();
    StringSource(blk, true, new HashFilter(hash, new StringSink(H)));
}
```

