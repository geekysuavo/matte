
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_SYMBOLS_H__
#define __MATTE_SYMBOLS_H__

/* include the object and ast-node headers. */
#include <matte/object.h>

/* IS_SYMBOLS: macro to check that an object is a matte symbol table.
 */
#define IS_SYMBOLS(obj) \
  MATTE_TYPE_CHECK(obj, symbols_type())

/* commonly used combinations of symbol types.
 */
#define SYMBOL_TEMP_VAR        (SYMBOL_TEMP   | SYMBOL_VAR)
#define SYMBOL_GLOBAL_VAR      (SYMBOL_GLOBAL | SYMBOL_VAR)
#define SYMBOL_STATIC_VAR      (SYMBOL_STATIC | SYMBOL_VAR)
#define SYMBOL_GLOBAL_FUNC     (SYMBOL_GLOBAL | SYMBOL_FUNC)
#define SYMBOL_GLOBAL_INT      (SYMBOL_GLOBAL | SYMBOL_VAR | SYMBOL_INT)
#define SYMBOL_GLOBAL_FLOAT    (SYMBOL_GLOBAL | SYMBOL_VAR | SYMBOL_FLOAT)
#define SYMBOL_GLOBAL_COMPLEX  (SYMBOL_GLOBAL | SYMBOL_VAR | SYMBOL_COMPLEX)
#define SYMBOL_GLOBAL_STRING   (SYMBOL_GLOBAL | SYMBOL_VAR | SYMBOL_STRING)
#define SYMBOL_LITERAL \
  (SYMBOL_INT | SYMBOL_FLOAT | SYMBOL_COMPLEX | SYMBOL_STRING)

/* Symbols: pointer to a struct _Symbols. */
typedef struct _Symbols *Symbols;
struct _ObjectType Symbols_type;

/* SymbolType: an enum _SymbolType. */
typedef enum _SymbolType SymbolType;

/* SymbolData: a union _SymbolData. */
typedef union _SymbolData SymbolData;

/* _SymbolType: enumeration that holds the types of symbols that may
 * be found within a symbol table.
 */
enum _SymbolType {
  /* used for blanket symbol lookups. */
  SYMBOL_ANY       = 0x0000,

  /* literal symbols. */
  SYMBOL_INT       = 0x0001,
  SYMBOL_FLOAT     = 0x0002,
  SYMBOL_COMPLEX   = 0x0004,
  SYMBOL_STRING    = 0x0008,

  /* variable symbols. */
  SYMBOL_VAR       = 0x0010,
  SYMBOL_ARGIN     = 0x0020,
  SYMBOL_ARGOUT    = 0x0040,

  /* function symbols. */
  SYMBOL_FUNC      = 0x0080,

  /* symbol state/scope. */
  SYMBOL_GLOBAL    = 0x0100,
  SYMBOL_STATIC    = 0x0200,
  SYMBOL_TEMP      = 0x0400
};

/* _SymbolData: union that holds any literal or identifier data value
 * that may conceivably be contained within an abstract syntax tree node.
 */
union _SymbolData {
  long iv;              /* integer data.              */
  double fv;            /* float data.                */
  complex double cv;    /* complex data.              */
  char *sv;             /* identifier or string data. */
};

/* Symbols: structure for holding a symbol table.
 */
struct _Symbols {
  /* base object. */
  OBJECT_BASE;

  /* @sym_type: array of basic types of the symbols.
   * @sym_data: array of data values of the symbols.
   * @own_data: whether the symbol has allocated data.
   * @sym_name: array of string names of the symbols.
   * @n: number of user-defined symbols.
   * @nt: number of temporary symbols.
   */
  SymbolType *sym_type;
  SymbolData *sym_data;
  char **sym_name;
  bool *own_data;
  long n, nt;
};

/* function declarations (symbols.c): */

ObjectType symbols_type (void);

Symbols symbols_new (Zone z, Object args);

void symbols_delete (Zone z, Symbols syms);

long symbols_find (Symbols syms, SymbolType stype, const char *sname, ...);

long symbols_add (Symbols syms, SymbolType stype, const char *sname, ...);

int symbol_has_type (Symbols syms, long index, SymbolType stype);

const char *symbol_name (Symbols syms, long index);

long symbol_int (Symbols syms, long index);

double symbol_float (Symbols syms, long index);

complex double symbol_complex (Symbols syms, long index);

const char *symbol_string (Symbols syms, long index);

#endif /* !__MATTE_SYMBOLS_H__ */

