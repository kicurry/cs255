#ifndef HASH_MPZ_H_
#define HASH_MPZ_H_

#include <cstddef>
#include <gmpxx.h>
#include "MurmurHash3.h"

template <>
struct std::hash<mpz_srcptr>
{
    size_t operator()(const mpz_srcptr x) const;
};

template <>
struct std::hash<mpz_t>
{
    size_t operator()(const mpz_t x) const;
};

template <>
struct std::hash<mpz_class>
{
    size_t operator()(const mpz_class &x) const;
};

size_t std::hash<mpz_srcptr>::operator()(const mpz_srcptr x) const
{
    uint32_t seed = static_cast<uint32_t>(984124);
    uint32_t res;
    MurmurHash3_x86_32(x->_mp_d, x->_mp_size * sizeof(mp_limb_t), seed, &res);
    return res;
}

size_t std::hash<mpz_t>::operator()(const mpz_t x) const
{
    return hash<mpz_srcptr>{}(static_cast<mpz_srcptr>(x));
}

size_t std::hash<mpz_class>::operator()(const mpz_class &x) const
{
    return hash<mpz_srcptr>{}(x.get_mpz_t());
}

#endif /* HASH_MPZ_H_ */