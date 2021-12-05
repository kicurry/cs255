#include <iostream>
#include <cassert>
#include <osrng.h>
#include <aes.h>
#include <hex.h>
#include <files.h>

using namespace CryptoPP;
using namespace std;

//
// Global Printer: bytes -> hex encoded
//
static HexEncoder encoder(new FileSink(cout));

//
// Global Decoder
//
static string decoded;
static HexDecoder decoder(new StringSink(decoded));

void dump_bytes(byte *bytes, size_t size, string label)
{
    cout << label << ": ";
    encoder.Put(bytes, size);
    encoder.MessageEnd();
    cout << endl;
}

class CTR
{
public:
    CTR() : iv_(nullptr), key_(nullptr), enable_test_(false) {}

    CTR(bool test) : iv_(nullptr), key_(nullptr), enable_test_(test) {}

    ~CTR()
    {
        if (iv_ != nullptr)
        {
            delete iv_;
        }
        delete key_;
    }

    void set_key(string key_str);

    // choose a random value in data block space
    void set_iv();

    // just output ciphertext
    void E(string msg_str);

    // just output plain_text
    void D(string cipher_text_str);

private:
    SecByteBlock *iv_;
    SecByteBlock *key_;
    AESEncryption aes_encryptor_;
    // only for test
    bool enable_test_;
};

void CTR::set_key(string key_str)
{
    //
    // 1. decode input key(16 base) to bytes
    // 2. Set AES's key (16 bytes)
    assert(!key_str.empty());
    decoded.clear();
    decoder.Put((byte *)key_str.data(), key_str.size()); // data(): get string's internal data pointer
    decoder.MessageEnd();
    key_ = new SecByteBlock(reinterpret_cast<const byte *>(decoded.data()), AES::DEFAULT_KEYLENGTH);
    dump_bytes(key_->data(), key_->size(), "AES's key");

    // Initialize AES Encryption & Decryption
    aes_encryptor_.SetKey(key_->data(), key_->size());
}

void CTR::D(string cipher_text_str)
{
    assert(key_ != NULL);

    // transform hex encoded string to bytes
    vector<SecByteBlock *> blks;
    decoded.clear();
    decoder.Put((byte *)cipher_text_str.data(), cipher_text_str.size());
    decoder.MessageEnd();

    // set last data block true size
    size_t last_size = decoded.size() % AES::BLOCKSIZE;
    if (last_size == 0)
    {
        last_size = AES::BLOCKSIZE;
    }
    // transform bytes to data blocks
    for (size_t i = 0; i < decoded.size(); i += AES::BLOCKSIZE)
    {
        auto msg_block = new SecByteBlock(reinterpret_cast<const byte *>(&decoded[i]), AES::BLOCKSIZE);
        blks.push_back(msg_block);
    }

    dump_bytes(blks[0]->data(), blks[0]->size(), "IV: ");
    cout << "# of bytes: " << decoded.size() - AES::BLOCKSIZE << endl;

    SecByteBlock iv_register(blks[0]->data(), blks[0]->size());
    for (size_t i = 1; i < blks.size(); i++)
    {
        if (enable_test_)
        { // print current IV
            dump_bytes(iv_register, iv_register.size(), "\nIV + " + to_string(i - 1));
        }

        SecByteBlock out_register(AES::BLOCKSIZE);
        aes_encryptor_.ProcessBlock(iv_register, out_register);
        if (enable_test_)
        {
            dump_bytes(out_register, out_register.size(), "XOR round" + to_string(i - 1));
        }
        ArraySource(blks[i]->data(), blks[i]->size(), true, new ArrayXorSink(out_register, out_register.size()));

        // print
        size_t length = out_register.size();
        if (i + 1 == blks.size())
        {
            length = last_size;
        }
        for (size_t j = 0; j < length; j++)
        {
            cout << out_register[j];
        }

        // increment IV by one
        IncrementCounterByOne(iv_register, iv_register.size());
    }
}

int main()
{
    string key = "36f18357be4dbd77f050515c73fcf9f2";
    string c1 = "69dda8455c7dd4254bf353b773304eec0ec7702330098ce7f7520d1cbbb20fc388d1b0adb5054dbd7370849dbf0b88d393f252e764f1f5f7ad97ef79d59ce29f5f51eeca32eabedd9afa9329";
    string c2 = "770b80259ec33beb2561358a9f2dc617e46218c0a53cbeca695ae45faa8952aa0e311bde9d4e01726d3184c34451";

    CTR ctr_cipher;
    ctr_cipher.set_key(key);
    ctr_cipher.D(c1);
    cout << endl;

    ctr_cipher.D(c2);
    cout << endl;

    return 0;
}