// 
// Test 'mpz_class' hash table
//

#include "hash_mpz.h"
#include <gmpxx.h>
#include <unordered_map>
#include <iostream>

int main()
{
    std::unordered_map<mpz_class, int> H;

    mpz_class a, b, c;
    a = "123456789987654321123456789987654321123456789987654321123456789987654321";
    b = "123456789123456789123456789987654321123456789987654321123456789987654321";
    c = "123456789987654321123456789987654321123456789987654321123456789987654321";

    H[a] = 1;
    H[b] = 2;
    H[c] = 3;

    std::cout << a << std::endl << b << std::endl << c << std::endl;
    std::cout << H[a] << " " << H[b] << " " << H[c] << std::endl;

    return 0;
}