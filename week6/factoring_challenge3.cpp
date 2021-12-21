#include <iostream>
#include <gmpxx.h>

int main()
{
    mpz_class N;
    N = "72006226374735042527956443552558373833808445147399984182665305798191"
        "63556901883377904234086641876639384851752649940178970835240791356868"
        "77441155132015188279331812309091996246361896836573643119174094961348"
        "52463970788523879939683923036467667022162701835329944324119217381272"
        "9276147530748597302192751375739387929";
    // N = "2039652913367"; // 1166083*1749149

    // A = 2*floor(sqrt(6N)) + 1
    mpz_class Np = 6 * N; // N' = 6N
    mpz_class A;
    mpz_sqrt(A.get_mpz_t(), Np.get_mpz_t()); // A = floor(sqrt(N'))
    A = 2 * A + 1;                           // A = 2 * A + 1

    mpz_class p, q;
    mpz_class d = A * A - 4 * Np;

    // x' = floor(sqrt(d))
    mpz_class xp;
    mpz_sqrt(xp.get_mpz_t(), d.get_mpz_t());

    q = A + xp;
    mpz_class res, reminder;
    mpz_cdiv_qr_ui(res.get_mpz_t(), reminder.get_mpz_t(), q.get_mpz_t(), 4);
    if (reminder == 0)
    {
        q = res;
    }
    else
    {
        q = (A - xp) / 4;
    }
    p = (A - 2 * q) / 3;

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