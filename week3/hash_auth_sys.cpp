#include <iostream>
#include <files.h>

using namespace std;
using namespace CryptoPP;

int main()
{
    string str("12345");
    SecByteBlock blk1((byte *)str.data(), str.size());
    AlignedSecByteBlock blk2((byte *)str.data(), str.size());
    cout << blk1.size() << endl;
    cout << blk2.size() << endl;
    return 0;
}