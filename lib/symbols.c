
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the symbol table header. */
#include <matte/symbols.h>

/* symbols_type(): return a pointer to the symbol table object type.
 */
ObjectType symbols_type (void) {
  /* return the struct address. */
  return &Symbols_type;
}

/* symbols_new(): allocate a new matte symbol table.
 *
 * returns:
 *  newly allocated and initialized matte symbol table.
 */
Symbols symbols_new (Object args) {
  /* allocate a new table. */
  Symbols syms = (Symbols) Symbols_type.fn_alloc(&Symbols_type);
  if (!syms)
    return NULL;

  /* initialize the symbol array. */
  syms->sym_type = NULL;
  syms->sym_data = NULL;
  syms->own_data = NULL;
  syms->sym_name = NULL;
  syms->nt = 0;
  syms->n = 0;

  /* return the new table. */
  return syms;
}

/* symbols_free(): free all memory associated with a matte symbol table.
 *
 * arguments:
 *  @syms: matte symbol table to free.
 */
void symbols_free (Symbols syms) {
  /* return if the table is null. */
  if (!syms)
    return;

  /* loop over the symbols. */
  for (long i = 0; i < syms->n; i++) {
    /* free the symbol name. */
    free(syms->sym_name[i]);

    /* if the symbol has allocated data, free it. */
    if (syms->own_data[i])
      free(syms->sym_data[i].sv);
  }

  /* free the arrays of symbol names, types, and data values. */
  free(syms->sym_type);
  free(syms->sym_data);
  free(syms->own_data);
  free(syms->sym_name);
}

/* symbols_find(): lookup a symbol in a matte symbol table.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @stype: symbol type to lookup.
 *  @sname: symbol name to lookup.
 *  @...: optional symbol data.
 *
 * returns:
 *  on successful symbol lookup, the one-based symbol index
 *  is returned. otherwise, zero (0) is returned.
 */
long symbols_find (Symbols syms, SymbolType stype, const char *sname, ...) {
  /* declare variables for reading the optional data argument. */
  SymbolData sdata;
  va_list vl;

  /* return 'not found' if the symbol table is null. */
  if (!syms)
    return 0;

  /* check if the symbol type implies an extra argument. */
  if (stype & SYMBOL_LITERAL) {
    /* read the extra argument. */
    va_start(vl, sname);
    sdata = (SymbolData) va_arg(vl, SymbolData);
    va_end(vl);

    /* search based on the symbol type. */
    if (stype & SYMBOL_INT) {
      /* search for an integer literal. */
      for (long i = 0; i < syms->n; i++) {
        if (syms->sym_type[i] & stype && syms->sym_data[i].iv == sdata.iv)
          return i + 1;
      }
    }
    else if (stype & SYMBOL_FLOAT) {
      /* search for a float literal. */
      for (long i = 0; i < syms->n; i++) {
        if (syms->sym_type[i] & stype && syms->sym_data[i].fv == sdata.fv)
          return i + 1;
      }
    }
    else if (stype & SYMBOL_COMPLEX) {
      /* search for a complex literal. */
      for (long i = 0; i < syms->n; i++) {
        if (syms->sym_type[i] & stype && syms->sym_data[i].cv == sdata.cv)
          return i + 1;
      }
    }
    else if (stype & SYMBOL_STRING) {
      /* search for a string literal. */
      for (long i = 0; i < syms->n; i++) {
        if (syms->sym_type[i] & stype &&
            strcmp(syms->sym_data[i].sv, sdata.sv) == 0)
          return i + 1;
      }
    }
  }

  /* fail if the symbol name is null. */
  if (!sname)
    return 0;

  /* search for the symbol in the array. */
  for (long i = 0; i < syms->n; i++) {
    /* on type and name match, return the one-based index. */
    if ((stype == SYMBOL_ANY || syms->sym_type[i] & stype) &&
        strcmp(syms->sym_name[i], sname) == 0)
      return i + 1;
  }

  /* return 'not found' */
  return 0;
}

/* symbols_add(): add a symbol into a matte symbol table.
 *
 * arguments:
 *  @syms: matte symbol table to modify.
 *  @stype: symbol type to register.
 *  @sname: symbol name to register.
 *  @...: optional symbol data.
 *
 * returns:
 *  on successful symbol addition/lookup, the one-based symbol index
 *  is returned. otherwise, zero (0) is returned.
 */
long symbols_add (Symbols syms, SymbolType stype, const char *sname, ...) {
  /* declare required variables:
   *  @sid: symbol index from checking for pre-existing symbols.
   *  @vl: variable-length argument list for literal data.
   */
  long sid;
  va_list vl;
  SymbolData sdata;

  /* return failure if the symbol table is null. */
  if (!syms)
    return 0;

  /* check if the symbol type implies an extra data argument. */
  if (stype & SYMBOL_LITERAL) {
    /* read the extra argument. */
    va_start(vl, sname);
    sdata = (SymbolData) va_arg(vl, SymbolData);
    va_end(vl);

    /* search for a literal symbol. */
    sid = symbols_find(syms, stype, sname, sdata);
  }
  else {
    /* perform a search without data. */
    sid = symbols_find(syms, stype, sname);
  }

  /* if the symbol is already in the table, return its index. */
  if (sid) return sid;

  /* increment the symbol table size. */
  sid = syms->n++;

  /* reallocate the symbol type array. */
  syms->sym_type = (SymbolType*)
    realloc(syms->sym_type, syms->n * sizeof(SymbolType));

  /* reallocate the symbol data array. */
  syms->sym_data = (SymbolData*)
    realloc(syms->sym_data, syms->n * sizeof(SymbolData));

  /* reallocate the symbol ownership array. */
  syms->own_data = (bool*)
    realloc(syms->own_data, syms->n * sizeof(bool));

  /* reallocate the symbol name array. */
  syms->sym_name = (char**)
    realloc(syms->sym_name, syms->n * sizeof(char*));

  /* check if reallocation failed. */
  if (!syms->sym_type || !syms->sym_data ||
      !syms->own_data || !syms->sym_name)
    fail("unable to reallocate array");

  /* store the symbol type; initialize the symbol data. */
  syms->sym_type[sid] = stype;
  syms->sym_data[sid].iv = 0L;
  syms->own_data[sid] = false;

  /* check if the symbol name is defined. */
  if (sname && !(stype & SYMBOL_TEMP)) {
    /* store the symbol name. */
    syms->sym_name[sid] = strdup(sname);
  }
  else {
    /* generate a new symbol name. */
    char *sbuf = (char*) malloc(32 * sizeof(char));
    if (sbuf) snprintf(sbuf, 32, "_t%ld", syms->nt);

    /* store the symbol name. */
    syms->sym_name[sid] = sbuf;
    syms->nt++;
  }

  /* check if symbol data should be stored. */
  if (stype & SYMBOL_LITERAL) {
    /* check if string allocation is required. */
    if (stype & SYMBOL_STRING) {
      syms->sym_data[sid].sv = strdup(sdata.sv);
      syms->own_data[sid] = true;
    }
    else
      syms->sym_data[sid] = sdata;
  }

  /* return success. */
  return sid + 1;
}

/* symbol_has_type(): check the type of a registered symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline int symbol_has_type (Symbols syms, long index, SymbolType stype) {
  /* return whether the symbol matches the type combination. */
  return (syms && index >= 0 && index < syms->n ?
          syms->sym_type[index] & stype : 0);
}

/* symbol_name(): return the name string of a registered symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline const char *symbol_name (Symbols syms, long index) {
  /* return the symbol name string. */
  return (syms && index >= 0 && index < syms->n ?
          syms->sym_name[index] : NULL);
}

/* symbol_int(): return the integer data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline long symbol_int (Symbols syms, long index) {
  /* return the symbol data as an integer. */
  return (syms && index >= 0 && index < syms->n ?
          syms->sym_data[index].iv : 0L);
}

/* symbol_float(): return the float data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline double symbol_float (Symbols syms, long index) {
  /* return the symbol data as a float. */
  return (syms && index >= 0 && index < syms->n ?
          syms->sym_data[index].fv : 0.0);
}

/* symbol_complex(): return the complex data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline complex double symbol_complex (Symbols syms, long index) {
  /* return the symbol data as a complex. */
  return (syms && index >= 0 && index < syms->n ?
          syms->sym_data[index].cv : 0.0);
}

/* symbol_string(): return the string data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline const char *symbol_string (Symbols syms, long index) {
  /* return the symbol data as a string. */
  return (syms && index >= 0 && index < syms->n && syms->own_data[index] ?
          syms->sym_data[index].sv : NULL);
}

/* Symbols_type: object type structure for matte symbol tables.
 */
struct _ObjectType Symbols_type = {
  "Symbols",                           /* name       */
  sizeof(struct _Symbols),             /* size       */
  0,                                   /* precedence */

  (obj_constructor) symbols_new,       /* fn_new     */
  (obj_allocator)   object_alloc,      /* fn_alloc   */
  (obj_destructor)  symbols_free,      /* fn_dealloc */
  NULL,                                /* fn_disp    */

  NULL,                                /* fn_plus       */
  NULL,                                /* fn_minus      */
  NULL,                                /* fn_uminus     */
  NULL,                                /* fn_times      */
  NULL,                                /* fn_mtimes     */
  NULL,                                /* fn_rdivide    */
  NULL,                                /* fn_ldivide    */
  NULL,                                /* fn_mrdivide   */
  NULL,                                /* fn_mldivide   */
  NULL,                                /* fn_power      */
  NULL,                                /* fn_mpower     */
  NULL,                                /* fn_lt         */
  NULL,                                /* fn_gt         */
  NULL,                                /* fn_le         */
  NULL,                                /* fn_ge         */
  NULL,                                /* fn_ne         */
  NULL,                                /* fn_eq         */
  NULL,                                /* fn_and        */
  NULL,                                /* fn_or         */
  NULL,                                /* fn_mand       */
  NULL,                                /* fn_mor        */
  NULL,                                /* fn_not        */
  NULL,                                /* fn_colon      */
  NULL,                                /* fn_ctranspose */
  NULL,                                /* fn_transpose  */
  NULL,                                /* fn_horzcat    */
  NULL,                                /* fn_vertcat    */
  NULL,                                /* fn_subsref    */
  NULL,                                /* fn_subsasgn   */
  NULL                                 /* fn_subsindex  */
};

