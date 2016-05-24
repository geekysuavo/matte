
WC=wc -l
RM=rm -f
TAR=tar czf
GREP=grep -RHni --color

DIR=matte
DATE=$(shell date +%Y%m%d)

TST=test.m
BIN=bin/matte
LIB=lib/libmatte.so

MATTE_ENV=env DYLD_LIBRARY_PATH=lib
MATTE_FLAGS=-Wp,-I. -Wp,-I/usr/local/atlas/include -Wl,-Llib

all: $(BIN) $(LIB)

$(BIN): $(LIB)
	@echo " MAKE $@"
	@$(MAKE) -sC bin

$(LIB):
	@echo " MAKE $@"
	@$(MAKE) -sC lib

test: all $(TST)
	@echo " TEST $(TST)"
	@$(MATTE_ENV) $(BIN) $(MATTE_FLAGS) $(TST)
	@$(RM) -r $(TST:.m=.dSYM)

clean:
	@$(MAKE) -sC bin clean
	@$(MAKE) -sC lib clean

again: clean all

lines:
	@echo " WC bin"
	@$(WC) bin/*.[ch]
	@echo " WC lib"
	@$(WC) lib/*.c
	@echo " WC matte"
	@$(WC) matte/*.h

fixme:
	@echo " FIXME bin"
	@$(GREP) fixme bin/*.[ch] || echo " None found"
	@echo " FIXME lib"
	@$(GREP) fixme lib/*.c || echo " None found"
	@echo " FIXME matte"
	@$(GREP) fixme matte/*.h || echo " None found"

dist: clean
	@echo " DIST $(DATE)"
	@$(RM) ../$(DIR)-$(DATE).tgz
	@$(TAR) ../$(DIR)-$(DATE).tgz *

