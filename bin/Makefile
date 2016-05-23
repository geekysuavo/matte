
CC=gcc
RM=rm -f
WC=wc -l
GREP=grep -RHni --color

ATLAS=/usr/local/atlas

CFLAGS=-Wall -Wextra -g -O3 -std=c99 -I.. -I$(ATLAS)/include
LDFLAGS=-L../lib -L$(ATLAS)/include
LIBS=-lmatte

BIN=matte

SRC=

OBJ= $(SRC:.c=.o) $(addsuffix .o,$(BIN))

.PHONY: all clean again lines fixme
.SUFFIXES: .c .o

all: $(BIN)

$(BIN): $(OBJ)
	@echo " LD $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)

.c.o:
	@echo " CC $^"
	@$(CC) $(CFLAGS) -c $^ -o $@

clean:
	@echo " CLEAN"
	@$(RM) $(BIN) $(OBJ)
	@$(RM) -r $(BIN).dSYM

again: clean all

lines:
	@echo " WC"
	@$(WC) *.[ch]

fixme:
	@echo " FIXME"
	@$(GREP) fixme *.[ch] || echo " None found"

