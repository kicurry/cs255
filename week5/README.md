### Makefile

target list:

- `./bin/DLOG_MIM`: source code for PA5
- `./bin/mpz_hash_table`: test for `mpz_class` hash
- `./bin/gmp_test`: GMP test demo

output directory of binary is `./bin` that no need to create by yourself and will be created automatically by command `make`.

All you need to do is install GMP library.



---

### Library: GMP for  C++

#### Install

1. Download the source code from gmplib offical [website](http://gmplib.org/).

2. See [documentation](https://gmplib.org/manual/Installing-GMP) for how to build and install GMP

   NOTE:

   - Please unzip directly under Linux

     I got something wrong when I unziped the source codes and copyed them to WSL-Ubuntu, say some executable binarys lost permission(configure, m4-ccas, and so on).  

   - to enable c++ support, we should set build options as follows,

     ```bash
     ./configure --enable-cxx
     ```

     see more details about build options in [documentation](https://gmplib.org/manual/Build-Options).

#### Compiler

```bash
g++ mycxxprog.cc -lgmpxx -lgmp
# mycxxprog.cc is your source code
```

NOTE: Reload library config if something wrong occurs,

```bash
ldconfig
```

### Interface for C++

More details in [documentation](https://gmplib.org/manual/C_002b_002b-Class-Interface). I just list some odds and ends.

