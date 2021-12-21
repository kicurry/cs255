#include <iostream>
#include <gmpxx.h>

int main()
{
    //
    // Float Point Operations are too slow
    //

    // mpf_class N("17976931348623159077293051907890247336179769789423065727343008115"
    //             "77326758055056206869853794492129829595855013875371640157101398586"
    //             "47833778606925583497541085196591615128057575940752635007475935288"
    //             "71082364994994077189561705436114947486504671101510156394068052754"
    //             "0071584560878577663743040086340742855278549092581");
    // // compute A = ceil(sqrt(N))
    // mpf_class A = ceil(sqrt(N));

    mpz_class N("17976931348623159077293051907890247336179769789423065727343008115"
                "77326758055056206869853794492129829595855013875371640157101398586"
                "47833778606925583497541085196591615128057575940752635007475935288"
                "71082364994994077189561705436114947486504671101510156394068052754"
                "0071584560878577663743040086340742855278549092581");

    //
    // Compute ceil(sqrt(N))
    // Implement ceiling function
    //
    mpz_class Ap, A;
    // Compute floor(sqrt(N))
    mpz_sqrt(Ap.get_mpz_t(), N.get_mpz_t());

    // check result of the floor function is equal to one of the ceiling function
    A = Ap;
    mpz_class A2 = Ap * Ap;
    if (N > A2)
    { // so sqrt(N) has been truncated, i.e. A = Ap + 1
        A += 1;
        A2 += (2 * Ap + 1); // A^2 = (Ap + 1)^2 = Ap^2 + 2 * Ap + 1
    }

    // Compute dalta = A^2 - N
    mpz_class delta = A2 - N;
    // Compute x = sqrt(dalta)
    mpz_class x;
    mpz_sqrt(x.get_mpz_t(), delta.get_mpz_t());

    mpz_class p, q;
    p = A - x, q = A + x;

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