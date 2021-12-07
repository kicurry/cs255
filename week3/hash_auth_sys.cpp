#include <iostream>
#include <files.h>
#include <sha.h>
#include <hex.h>

using namespace std;
using namespace CryptoPP;

size_t BLOCKSIZE = 1024;

int main()
{
    string content;
    FileSource("./6.1.intro.mp4_download", true,
               new StringSink(content));
    // FileSource("./6.2.birthday.mp4_download", true,
    //            new StringSink(content));
    // string hash_check2 = "03c08f4ee0b576fe319338139c045c89c3e8e9409633bea29442e21425006ea8";

    SHA256 hash;
    string H;

    // Misunderstanding!!
    // size_t true_block_size = BLOCKSIZE - hash.DigestSize();

    // size_t groups = content.size() / true_block_size;
    // size_t last_size = content.size() % true_block_size;
    // if (last_size + true_block_size <= BLOCKSIZE)
    // {
    //     groups--;
    //     last_size += true_block_size;
    // }
    // size_t i = groups * true_block_size;
    // string blk = content.substr(i) + H;
    // hash.Update((const byte *)&blk[0], blk.size());
    // H.clear();
    // H.resize(hash.DigestSize());
    // hash.Final((byte *)&H[0]);
    // i -= true_block_size;
    // for (; i; i -= true_block_size)
    // {
    //     blk = content.substr(i, true_block_size) + H;
    //     hash.Update((const byte *)&blk[0], blk.size());
    //     H.clear();
    //     H.resize(hash.DigestSize());
    //     hash.Final((byte *)&H[0]);
    // }

    for (int i = content.size() / BLOCKSIZE * BLOCKSIZE; i >= 0; i -= BLOCKSIZE)
    {
        string blk = content.substr(i, BLOCKSIZE) + H;
        H.clear();
        StringSource(blk, true, new HashFilter(hash, new StringSink(H)));
    }

    string encoded;
    StringSource(H, true,
                 new HexEncoder(
                     new StringSink(encoded)));
    cout << encoded << endl;

    return 0;
}