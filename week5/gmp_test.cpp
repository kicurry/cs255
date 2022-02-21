/*
 * Test usage of gmp
 */
#include <gmpxx.h>
#include <iostream>
#include <sstream>

typedef unsigned long long ULL;

int main()
{
    //
    // Convert integer to string
    //
    std::cout << "Test mpz_class to string:" << std::endl;
    mpz_class a, b, c;
    a = "134078079299425970995740249982058461274793658205923933"
        "77723561443721764030073546976801874298166903427690031"
        "858186486050853753882811946569946433649006084171";
    b = "11717829880366207009516117596335367088558084999998952205"
        "59997945906392949973658374667057217647146031292859482967"
        "5428279466566527115212748467589894601965568";
    c = "323947510405045044356526437872806578864909752095244"
        "952783479245297198197614329255807385693795855318053"
        "2878928001494706097394108577585732452307673444020333";

    // three ways to convert mpz_class to string

    // Method1: get raw string from mpz_class object by calling 'get_str'
    std::cout << "a:" << a.get_str() << std::endl;

    // Method2: for C++ style(overload operator <<)
    std::cout << "b:" << b << std::endl;

    // Method3: for C style
    std::cout << "c:" << mpz_get_str(nullptr, 10, c.get_mpz_t()) << std::endl;

    //
    // Test unsigned long long
    //
    std::cout << std::endl
              << "Test ULL to mpz_class:" << std::endl;
    mpz_class x, y;

    // error: no overload for ULL(unsigned long long)
    // x = 18446602493812015104;

    // link@https://gmplib.org/list-archives/gmp-discuss/2005-April/001606.html
    // Method1: transform ULL to string by stringstream
    std::stringstream ss;
    ULL _x = 13835058055282163712ULL;
    ss << _x;
    ss >> x;
    std::cout << "x = " << x << std::endl;
    ss.clear();

    // link@https://stackoverflow.com/questions/6248723/mpz-t-to-unsigned-long-long-conversion-gmp-lib
    // Method2: use mpz_import
    mpz_import(y.get_mpz_t(), 1, -1, sizeof(_x), 0, 0, &_x);
    std::cout << "y = " << y << std::endl;
    // see more details about mpz_import:
    // https://gmplib.org/manual/Integer-Import-and-Export
    // void mpz_import (mpz_t rop, size_t count, int order, size_t size, int endian, size_t nails, const void *op)
    // read 'count' word, each 'size' bytes;
    // order: mostly modern PC is little endian, so 'order' is -1;
    // endian: 0 for the native endianness of the host CPU;
    // nail: The most significant nails bits of each word are unused and set to zero, this can be 0 to produce full words.

    //
    // check 8^64 mod 0xFFFFFFFF00000001 = 1
    //
    std::cout << std::endl
              << "Check 8^64 mod 0xFFFFFFFF00000001 = 1" << std::endl;
    mpz_class g, N, m, res;

    g = 8;
    N = 64;
    ULL _m = 0xFFFFFFFF00000001ULL;
    ss << _m;
    ss >> m;
    ss.clear();
    std::cout << "m = " << m << std::endl;

    mpz_powm(res.get_mpz_t(), g.get_mpz_t(), N.get_mpz_t(), m.get_mpz_t());

    std::cout << "8^64 = " << res << std::endl;

    return 0;
}