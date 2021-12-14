SHELL = /bin/sh

# output
BINARY = ./bin

# compile options
CXX = g++
CFLAG = -I$(INCLUDE)
INCLUDE = hash_mpz.h
LIBS = -lgmpxx -lgmp

all: $(BINARY)/mpz_hash_table $(BINARY)/DLOG_MIM $(BINARY)/gmp_test

pre_output:
	mkdir -p ./bin

OBJS1 = MurmurHash3.o mpz_hash_table.o
_OBJS1 = $(BINARY)/MurmurHash3.o $(BINARY)/mpz_hash_table.o
$(BINARY)/mpz_hash_table: pre_output $(OBJS1)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS1) ${LIBS}

OBJS2 = MurmurHash3.o DLOG_MIM.o
_OBJS2 = $(BINARY)/MurmurHash3.o $(BINARY)/DLOG_MIM.o
$(BINARY)/DLOG_MIM: pre_output $(OBJS2)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS2) ${LIBS}

OBJS3 = gmp_test.o
_OBJS3 = $(BINARY)/gmp_test.o
$(BINARY)/gmp_test: pre_output $(OBJS3)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS3) ${LIBS}

clean:
	-rm -f $(BINARY)/*.o $(BINARY)/core $(BINARY)/*.core

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $(BINARY)/$@