
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_SYMBOLS_H__
#define __MATTE_SYMBOLS_H__

/* include the object header. */
#include <matte/object.h>

/* IS_SYMBOLS: macro to check that an object is a matte symbol table.
 */
#define IS_SYMBOLS(obj) \
  MATTE_TYPE_CHECK(obj, symbols_type())

/* Symbols: pointer to a struct _Symbols. */
typedef struct _Symbols *Symbols;
struct _ObjectType Symbols_type;

/* SymbolType: an enum _SymbolType. */
typedef enum _SymbolType SymbolType;

/* _SymbolType: enumeration that holds the types of symbols that may
 * be found within a symbol table.
 */
enum _SymbolType {
  SYMBOL_TYPE_ANY = 0,
  SYMBOL_TYPE_VAR,
  SYMBOL_TYPE_ARGIN,
  SYMBOL_TYPE_ARGOUT,
  SYMBOL_TYPE_FUNC
};

/* Symbols: structure for holding a symbol table.
 */
struct _Symbols {
  /* base object. */
  OBJECT_BASE;

  /* @sym_type: array of basic types of the symbols.
   * @sym_name: array of string names of the symbols.
   * @n: number of user-defined symbols.
   * @nt: number of temporary symbols.
   */
  SymbolType *sym_type;
  char **sym_name;
  long n, nt;
};

/* function declarations (symbols.c): */

ObjectType symbols_type (void);

Symbols symbols_new (Object args);

void symbols_free (Symbols syms);

long symbols_find (Symbols syms, SymbolType stype, const char *sname);

long symbols_add (Symbols syms, SymbolType stype, const char *sname);

#endif /* !__MATTE_SYMBOLS_H__ */

