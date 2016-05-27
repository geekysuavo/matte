
CC=gcc
RM=rm -f
WC=wc -l
GREP=grep -RHni --color

ATLAS=/usr/local/atlas

CFLAGS=-fPIC -g -O3 -std=c99 -I.. -I$(ATLAS)/include
CFLAGS+= -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
LDFLAGS=-shared -L$(ATLAS)/lib
LIBS=-ltatlas -lm

LIB=libmatte.so

SRC=  errors.c builtins.c object.c except.c object-list.c struct.c string.c
SRC+= int.c range.c float.c complex.c vector.c complex-vector.c
SRC+= scanner.c scanner-token.c parser.c ast.c symbols.c compiler.c
OBJ=$(SRC:.c=.o)

.PHONY: all clean again lines fixme
.SUFFIXES: .c .o

all: $(LIB)

$(LIB): $(OBJ)
	@echo " LD $@"
	@$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

.c.o:
	@echo " CC $^"
	@$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@echo " CLEAN"
	@$(RM) $(LIB) $(OBJ)
	@$(RM) -r $(LIB).dSYM

again: clean all

lines:
	@echo " WC"
	@$(WC) *.[ch]

fixme:
	@echo " FIXME"
	@$(GREP) fixme *.[ch] || echo " None found"

