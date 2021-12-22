SHELL = /bin/sh

# output
BINARY = ./bin

# compile options
CXX = g++
CFLAGS = $(INCLUDE)
INCLUDE = 
LIBS = -lgmpxx -lgmp

all: $(BINARY)/fc1

pre_output:
	mkdir -p ./bin

OBJS1 = factoring_challenge1.o
_OBJS1 = $(BINARY)/factoring_challenge1.o
$(BINARY)/fc1: pre_output $(OBJS1)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS1) ${LIBS}

OBJS2 = factoring_challenge2.o
_OBJS2 = $(BINARY)/factoring_challenge2.o
$(BINARY)/fc2: pre_output $(OBJS2)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS2) ${LIBS}

OBJS3 = factoring_challenge3.o
_OBJS3 = $(BINARY)/factoring_challenge3.o
$(BINARY)/fc3: pre_output $(OBJS3)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS3) ${LIBS}

OBJS4 = rsa_challenge.o
_OBJS4 = $(BINARY)/rsa_challenge.o
$(BINARY)/rsa_challenge: pre_output $(OBJS4)
	$(CXX) ${CFLAGS} -o $@ $(_OBJS4) ${LIBS}

clean:
	-rm -f $(BINARY)/*.o $(BINARY)/core $(BINARY)/*.core

.cpp.o:
	${CXX} ${CFLAGS} -c $< -o $(BINARY)/$@