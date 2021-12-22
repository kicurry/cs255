#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <gmpxx.h>

void HexToBytes(const std::string &hex, std::vector<char> &bytes)
{
    for (unsigned int i = 0; i < hex.length(); i += 2)
    {
        std::string byteString = hex.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
}

int main()
{
    mpz_class N("17976931348623159077293051907890247336179769789423065727343008115"
                "77326758055056206869853794492129829595855013875371640157101398586"
                "47833778606925583497541085196591615128057575940752635007475935288"
                "71082364994994077189561705436114947486504671101510156394068052754"
                "0071584560878577663743040086340742855278549092581");

    // Using factorization got by factoring_challenge1.cpp
    mpz_class p("13407807929942597099574024998205846127479365820592393377723561443"
                "72176403007366276889111161436232699867504054609433932083841952337"
                "5986027530441562135724301");
    mpz_class q("13407807929942597099574024998205846127479365820592393377723561443"
                "72176403007377856098034893055775056966004923400219259082308516394"
                "0025485114449475265364281");

    // Euler's theorem
    mpz_class phi_N = (p - 1) * (q - 1);

    // By RSA's definition,
    // d = e^(-1) (mod phi(N))
    mpz_class e("65537");
    mpz_class d;
    int ret = mpz_invert(d.get_mpz_t(), e.get_mpz_t(), phi_N.get_mpz_t());
    assert(ret != 0);

    // decryption
    mpz_class cipher_text_base_10("220964518674103817763065611348834180174100697878"
                                  "928310717318391436761356001205380042823296504735"
                                  "094243439462197515122564658399679428894607645420"
                                  "405815647489880137348641204523252293201764879166"
                                  "664029975091887299716905260832220677716000193292"
                                  "608700095799937240774589677736978175712672299511"
                                  "48662959627934791540");
    mpz_class plain_text_pcks1_encoded_base_10;
    mpz_powm(plain_text_pcks1_encoded_base_10.get_mpz_t(),
             cipher_text_base_10.get_mpz_t(), d.get_mpz_t(), N.get_mpz_t());

    // Decode from PCKS1.5 encoding
    std::stringstream ss;
    // mpz_class overload "<<", so need not convert decimal string to hex string like,
    // ss << std::hex << plain_text_pcks1_encoded_base_10.get_str() << std::endl;
    ss << std::hex << plain_text_pcks1_encoded_base_10 << std::endl;
    std::string plain_text_pcks1_encoded_base_16;
    ss >> plain_text_pcks1_encoded_base_16;

    int pos = plain_text_pcks1_encoded_base_16.find("00");
    assert(pos != std::string::npos);
    pos += 2;

    std::string plain_text_base_16;
    plain_text_base_16 = plain_text_pcks1_encoded_base_16.substr(pos);

    // Decode hex string to ASCII string
    std::vector<char> plain_text;
    HexToBytes(plain_text_base_16, plain_text);

    // Output result
    std::cout << "PT pcks1.5 encoded: 0x" << plain_text_pcks1_encoded_base_16 << std::endl;
    std::cout << "PT hex: 0x" << plain_text_base_16 << std::endl;
    std::cout << "PT: ";
    for (auto c : plain_text)
    {
        std::cout << c;
    }
    std::cout << std::endl;

    return 0;
}