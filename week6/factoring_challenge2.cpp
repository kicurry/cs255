#include <iostream>
#include <gmpxx.h>

int main()
{
    mpz_class N;
    N = "6484558428080716696628242653467722787263437207069762630604390703787"
        "9730861808111646271401527606141756919558732184025452065542490671989"
        "2428844841839353281972988531310511738648965962582821502504990264452"
        "1008852816733037111422964210278402893076574586452336833570778346897"
        "15838646088239640236866252211790085787877";

    // A' = floor(sqrt(N))
    mpz_class Ap;
    mpz_sqrt(Ap.get_mpz_t(), N.get_mpz_t());

    mpz_class p, q;
    for (unsigned i = 0; i < (1 << 20) - 1; i++)
    {
        mpz_class A = Ap + i;
        mpz_class d = A * A - N;

        if (d < 0)
        {
            continue;
        }

        // x' = floor(sqrt(d))
        mpz_class xp;
        mpz_sqrt(xp.get_mpz_t(), d.get_mpz_t());

        if (xp * xp == d)
        {
            p = A - xp, q = A + xp;
            break;
        }
    }

    // output the result
    std::cout << "p = " << p << std::endl;
    std::cout << "q = " << q << std::endl;

    // Check the answer
    puts("Check the answer");
    mpz_class real = p * q;
    if (real == N)
    {
        puts("Answer is correct.");
        std::cout << "N = " << p << "*" << q << std::endl;
    }
    else
    {
        std::cout << "expected N = " << N << std::endl;
        std::cout << "but get p*q = " << real << std::endl;
    }

    return 0;
}