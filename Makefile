
WC=wc -l
RM=rm -f
TAR=tar czf
GREP=grep -RHni --color

DIR=matte
DATE=$(shell date +%Y%m%d)

BIN=bin/matte
LIB=lib/libmatte.so

MATTE_ENV=env DYLD_LIBRARY_PATH=lib
MATTE_FLAGS=-I. -I/usr/local/atlas/include -Llib

TST=test.m
TST_C=$(TST:.m=.c)
TST_EXE=$(basename $(TST))

all: $(BIN) $(LIB)

$(BIN): $(LIB)
	@echo " MAKE $@"
	@$(MAKE) -sC bin

$(LIB):
	@echo " MAKE $@"
	@$(MAKE) -sC lib

test-c: $(TST_C)
test-exe: $(TST_EXE)

test-mem: $(TST)
	@echo " TEST MEM $(TST)"
	@$(MATTE_ENV) $(BIN) $(MATTE_FLAGS) $(TST)

$(TST_C): $(TST)
	@echo " TEST C   $(TST)"
	@$(MATTE_ENV) $(BIN) $(MATTE_FLAGS) -c -o $(TST_C) $(TST)

$(TST_EXE): $(TST)
	@echo " TEST EXE $(TST)"
	@$(MATTE_ENV) $(BIN) $(MATTE_FLAGS) -o $(TST_EXE) $(TST)
	@$(MATTE_ENV) ./$(TST_EXE)

clean:
	@$(MAKE) -sC bin clean
	@$(MAKE) -sC lib clean
	@$(RM) $(TST_C) $(TST_EXE)

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

