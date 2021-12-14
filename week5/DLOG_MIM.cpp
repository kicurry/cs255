// Solution the discrete log modulo a prime p using "meeting in the middle"
//
//  x <= 2^40, so we can represent x as 2^20-Based encode: x = x0 * B + x1.
//  Then we can rewrite h = g^x = g^(x0 * B + x1) as:
//                  h / (g^x1) = (g^B)^x0
//
// 1. First build a hash table of all possible values of the left hand side
// 'h/(g^x1)' for x1 = 0,1,...,2^20
// ​
// 2. Then for each value x0 = 0,1,2,.., 2^20 check if the right hand side '(g^B)^x0'
// is in this hash table.  If so, then you have found a solution (x_0,x_1) from which
// you can compute the required x as x =  x0 * B + x1

#include <iostream>
#include <cstdio>
#include <gmpxx.h>
#include <unordered_map>
#include "hash_mpz.h"

#define BIT_LENGTH 153
#define MID 20

typedef unsigned long UL;
typedef unsigned long long ULL;

int main()
{
    mpz_class p;
    mpz_class g;
    mpz_class h;
    // initilize and assign input
    p = "134078079299425970995740249982058461274793658205923933"
        "77723561443721764030073546976801874298166903427690031"
        "858186486050853753882811946569946433649006084171";
    g = "11717829880366207009516117596335367088558084999998952205"
        "59997945906392949973658374667057217647146031292859482967"
        "5428279466566527115212748467589894601965568";
    h = "323947510405045044356526437872806578864909752095244"
        "952783479245297198197614329255807385693795855318053"
        "2878928001494706097394108577585732452307673444020333";

    UL B = 1 << 20;
    //
    // First build hash table
    //
    // becasue 0 <= x1 < 2^20, we can just use unsigned int to store it.
    std::unordered_map<mpz_class, UL> hash_table;
    for (UL i = 0; i < B; i++)
    {
        mpz_t left;
        mpz_init(left);

        // Compute: h / (g^x1)
        // 1. left := g^x1
        mpz_powm_ui(left, g.get_mpz_t(), i, p.get_mpz_t());

        // 2. left := left^(-1)
        mpz_invert(left, left, p.get_mpz_t());

        // 3. left := h * left
        mpz_mul(left, h.get_mpz_t(), left);
        // Caveat: don't forget to do Modular!!!
        mpz_mod(left, left, p.get_mpz_t());

        // insert into hash table
        hash_table[mpz_class(left)] = i;
    }
    std::cout << "Building hash table finished" << std::endl;

    //
    // Then Check
    //
    UL x0, x1;
    mpz_class right_tmp;
    // Compute right side: (g^B)^x0
    // 1. right := g^B
    mpz_powm_ui(right_tmp.get_mpz_t(), g.get_mpz_t(), B, p.get_mpz_t());
    for (UL i = 0; i < B; i++)
    {
        mpz_class right(right_tmp);
        // 2. right := right^x0
        mpz_powm_ui(right.get_mpz_t(), right.get_mpz_t(), i, p.get_mpz_t());

        auto res = hash_table.find(right);
        if (res != hash_table.end())
        {
            std::cout << res->first << std::endl;
            std::cout << right << std::endl;
            x1 = res->second;
            x0 = i;
            break;
        }
    }

    std::cout << "Met in the middle"
              << "(x0, x1) = "
              << "(" << x0 << ", " << x1 << ")" << std::endl;

    // Something error in check codes...
    // See notes!!
    ULL x = ((ULL)x0 << 20) + x1;
    std::cout << "x = " << x << std::endl;

    std::cout << "Start checking answer..." << std::endl;
    mpz_class res, _x;
    // Because gmplib does not support primitive type ULL, convert x to mpz_class for more computing
    mpz_import(_x.get_mpz_t(), 1, -1, sizeof(x), 0, 0, &x);

    // Compute g^x directly
    mpz_powm(res.get_mpz_t(), g.get_mpz_t(), _x.get_mpz_t(), p.get_mpz_t());

    // Compare and check
    std::cout << "Answer is ";
    if (res == h)
    {
        puts("correct.");
    }
    else
    {
        puts("false.");
        std::cout << "expect: " << h << std::endl;
        std::cout << "but get: " << res << std::endl;
    }

    return 0;
}