#include <iostream>
#include <cassert>
#include <osrng.h>
#include <aes.h>
#include <hex.h>
#include <files.h>
#include <modes.h>

using namespace CryptoPP;
using namespace std;

//
// Global PRG
//
static AutoSeededRandomPool prng;

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

class CBC
{
public:
    CBC() : enable_test_(false) {}
    CBC(bool test) : enable_test_(test) {}
    void init(string key_str, bool init_iv = false);

    void set_key(string key_str);

    // just output ciphertext
    void E(string msg_str);

    // just output plain_text
    void D(string cipher_text_str);

private:
    void block_transformation_filter(byte *in, size_t &length, bool is_forward);

    void trans_to_data_blocks(string str, vector<SecByteBlock *> &blks, bool is_forward = false);

private:
    SecByteBlock *iv_;
    SecByteBlock *key_;
    AESEncryption aes_encryptor_;
    AESDecryption aes_decryptor_;

    // only for test
    bool enable_test_;
    CBC_Mode<AES>::Encryption cbc_e_;
    CBC_Mode<AES>::Decryption cbc_d_;
};

void CBC::set_key(string str)
{
    //
    // 1. decode input key(16 base) to bytes
    // 2. Set AES's key (16 bytes)
    assert(!str.empty());
    decoded.clear();
    decoder.Put((byte *)str.data(), str.size()); // data(): get string's internal data pointer
    decoder.MessageEnd();
    key_ = new SecByteBlock(reinterpret_cast<const byte *>(decoded.data()), AES::DEFAULT_KEYLENGTH);
    dump_bytes(key_->data(), key_->size(), "AES's key");
}

void CBC::init(string key_str, bool init_iv)
{
    set_key(key_str);
    // Initialize AES Encryption & Decryption
    aes_encryptor_.SetKey(key_->data(), key_->size());
    aes_decryptor_.SetKey(key_->data(), key_->size());

    if (!init_iv)
    {
        return;
    }

    // Generate a random IV in data block spcae
    iv_ = new SecByteBlock(AES::BLOCKSIZE); // data block size: 16 bytes
    prng.GenerateBlock(iv_->data(), iv_->size());
    cbc_e_.SetKeyWithIV(key_->data(), key_->size(), iv_->data());
    cbc_d_.SetKeyWithIV(key_->data(), key_->size(), iv_->data());
}

void CBC::block_transformation_filter(byte *in, size_t &length, bool is_forward)
{
    size_t mandatory_size = AES::BLOCKSIZE;
    if (is_forward)
    { // Encryption
        size_t pad = mandatory_size;
        if (length && length < mandatory_size)
        { // corner case: length == 0
            pad -= length;
        }
        // PKCS padding
        memset(in + length, pad, pad);
    }
    else
    { // Decryption
        // check data block
        assert((length % mandatory_size) == 0);
        size_t pad = in[length - 1];
        if (pad < 1 || pad > mandatory_size)
        {
            cerr << "invalid ciphertext!" << endl;
            assert(false);
        }
        // filter padding
        length -= pad;
        memset(in + length, 0, pad);
    }
}

void CBC::trans_to_data_blocks(string in, vector<SecByteBlock *> &blks, bool is_forward)
{
    size_t mandatory_size = AES::BLOCKSIZE;
    for (size_t i = 0; i < in.size(); i += AES::BLOCKSIZE)
    {
        auto msg_block = new SecByteBlock(reinterpret_cast<const byte *>(&in[i]), AES::BLOCKSIZE);
        blks.push_back(msg_block);
    }

    if (is_forward)
    { // plaintext
        // transform last data block
        size_t last_size = in.size() % mandatory_size;
        if (last_size == 0)
        {
            blks.push_back(new SecByteBlock(mandatory_size));
        }
        block_transformation_filter(blks.back()->data(), last_size, is_forward);
    }

    if (enable_test_)
    {
        size_t i = 0;
        for (auto blk : blks)
        {
            dump_bytes(blk->data(), blk->size(), "blk[" + to_string(i) + "]");
            i++;
        }
    }
}

void CBC::E(string msg_str)
{
    assert(key_ != NULL);
    assert(iv_ != NULL);

    // decode input plaintext(16 base) to bytes
    vector<SecByteBlock *> msg;
    trans_to_data_blocks(msg_str, msg, true);

    // CBC Encryption
    SecByteBlock m_register(*iv_); // reg <- IV
    cout << "(IV, ciphertext): (";
    encoder.Put(iv_->data(), iv_->size());
    encoder.MessageEnd();
    cout << ", ";
    for (size_t i = 0; i < msg.size(); i++)
    {
        ArraySource((byte *)msg[i]->data(), msg[i]->size(), true, new ArrayXorSink(m_register, m_register.size()));
        if (enable_test_)
        {
            dump_bytes(m_register, m_register.size(), "\nXOR round" + to_string(i));
        }
        aes_encryptor_.ProcessBlock(m_register);

        // print c[i]
        encoder.Put(m_register, m_register.size());
        encoder.MessageEnd();
    }
    cout << ")" << endl;

    // Library CBC only for test
    if (enable_test_)
    {
        string cipher, encoded;
        StringSource(msg_str, true,
                     new StreamTransformationFilter(cbc_e_,
                                                    new StringSink(cipher)) // StreamTransformationFilter
        );                                                                  // StringSource

        StringSource(cipher, true,
                     new HexEncoder(
                         new StringSink(encoded)) // HexEncoder
        );                                        // StringSource
        cout << "library cbc: " << encoded << endl;
    }
}

void CBC::D(string cipher_text_str)
{
    assert(key_ != NULL);

    // transform hex encoded to bytes
    decoded.clear();
    decoder.Put((byte *)cipher_text_str.data(), cipher_text_str.size());
    decoder.MessageEnd();

    vector<SecByteBlock *> blks;
    trans_to_data_blocks(decoded, blks);

    dump_bytes(blks[0]->data(), blks[0]->size(), "IV: ");
    cout << "# of bytes: " << decoded.size() - AES::BLOCKSIZE << endl;

    for (size_t i = 1; i < blks.size(); i++)
    {
        SecByteBlock out_register(blks[i]->size());
        aes_decryptor_.ProcessBlock(blks[i]->data(), out_register);
        if (enable_test_)
        {
            dump_bytes(out_register, out_register.size(), "\ntmp[" + to_string(i - 1) + "]");
        }
        ArraySource(blks[i - 1]->data(), blks[i - 1]->size(), true, new ArrayXorSink(out_register, out_register.size()));

        // drop PKCS padding
        size_t length = out_register.size();
        if (i + 1 == blks.size())
        {
            block_transformation_filter(out_register, length, false);
        }

        // print
        for (size_t j = 0; j < length; j++)
        {
            cout << out_register[j];
        }
    }
}

int main()
{
    string key1 = "140b41b22a29beb4061bda66b6747e14";
    string key2 = "140b41b22a29beb4061bda66b6747e14";
    string cipher_text1 = "4ca00ff4c898d61e1edbf1800618fb2828a226d160dad07883d04e008a7897ee2e4b7465d5290d0c0e6c6822236e1daafb94ffe0c5da05d9476be028ad7c1d81";
    string cipher_text2 = "5b68629feb8606f9a6667670b75b38a5b4832d0f26e1ab7da33249de7d4afc48e713ac646ace36e872ad5fb8a512428a6e21364b0c374df45503473c5242a253";

    CBC cbc_cipher;

    //
    // Test Encryption
    //
    // string plain_text = "CBC Mode Test   ";
    // cbc_cipher.init(input_key, true);
    // cbc_cipher.E(plain_text);

    //
    // Tset Decryption
    //
    cbc_cipher.init(key1);
    cbc_cipher.D(cipher_text1);
    cout << endl;

    cbc_cipher.set_key(key2);
    cbc_cipher.D(cipher_text2);
    cout << endl;
    
    return 0;
}
