
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
  syms->sym_name = NULL;
  syms->sym_data = NULL;
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

  /* free the symbol names. */
  for (long i = 0; i < syms->n; i++)
    free(syms->sym_name[i]);

  /* free the symbol data values. */
  for (long i = 0; i < syms->n; i++)
    free(syms->sym_data[i]);

  /* free the arrays of symbol names, types, and data values. */
  free(syms->sym_name);
  free(syms->sym_type);
  free(syms->sym_data);
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
  /* declare required variables:
   *  @vl: variable-length argument list for literal data.
   */
  va_list vl;

  /* return 'not found' if the symbol table is null. */
  if (!syms)
    return 0;

  /* search based on the symbol type. */
  va_start(vl, sname);
  if (stype & SYMBOL_INT) {
    /* get the integer literal data. */
    long data = (long) va_arg(vl, long);
    va_end(vl);

    /* search for the symbol. */
    for (long i = 0; i < syms->n; i++) {
      if (!syms->sym_data[i]) continue;
      if (syms->sym_type[i] & stype &&
          *((long*) syms->sym_data[i]) == data)
        return i + 1;
    }
  }
  else if (stype & SYMBOL_FLOAT) {
    /* get the float literal data. */
    double data = (double) va_arg(vl, double);
    va_end(vl);

    /* search for the symbol. */
    for (long i = 0; i < syms->n; i++) {
      if (!syms->sym_data[i]) continue;
      if (syms->sym_type[i] & stype &&
          *((double*) syms->sym_data[i]) == data)
        return i + 1;
    }
  }
  else if (stype & SYMBOL_COMPLEX) {
    /* get the complex literal data. */
    complex double data = (complex double) va_arg(vl, complex double);
    va_end(vl);

    /* search for the symbol. */
    for (long i = 0; i < syms->n; i++) {
      if (!syms->sym_data[i]) continue;
      if (syms->sym_type[i] & stype &&
          *((complex double*) syms->sym_data[i]) == data)
        return i + 1;
    }
  }
  else if (stype & SYMBOL_STRING) {
    /* get the string literal data. */
    char *data = (char*) va_arg(vl, char*);
    va_end(vl);

    /* search for the symbol. */
    for (long i = 0; i < syms->n; i++) {
      if (!syms->sym_data[i]) continue;
      if (syms->sym_type[i] & stype &&
          strcmp((char*) syms->sym_data[i], data) == 0)
        return i + 1;
    }
  }

  /* end variable-length argument parsing. */
  va_end(vl);

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

  /* declare and initialize variables for holding literal data. */
  long data_int = 0L;
  double data_float = 0.0;
  complex double data_complex = 0.0;
  char *data_str = NULL;

  /* return failure if the symbol table is null. */
  if (!syms)
    return 0;

  /* check if the symbol type implies an extra data argument. */
  va_start(vl, sname);
  if (stype & SYMBOL_INT) {
    /* store the data and perform an integer literal search. */
    data_int = (long) va_arg(vl, long);
    sid = symbols_find(syms, stype, sname, data_int);
  }
  else if (stype & SYMBOL_FLOAT) {
    /* store the data and perform a float literal search. */
    data_float = (double) va_arg(vl, double);
    sid = symbols_find(syms, stype, sname, data_float);
  }
  else if (stype & SYMBOL_COMPLEX) {
    /* store the data and perform a complex literal search. */
    data_complex = (complex double) va_arg(vl, complex double);
    sid = symbols_find(syms, stype, sname, data_complex);
  }
  else if (stype & SYMBOL_STRING) {
    /* store the data and perform a string literal search. */
    data_str = (char*) va_arg(vl, char*);
    sid = symbols_find(syms, stype, sname, data_str);
  }
  else {
    /* perform a search without data. */
    sid = symbols_find(syms, stype, sname);
  }

  /* end variable-length argument handling. */
  va_end(vl);

  /* if the symbol is already in the table, return its index. */
  if (sid) return sid;

  /* increment the symbol table size. */
  syms->n++;

  /* reallocate the symbol type array. */
  syms->sym_type = (SymbolType*)
    realloc(syms->sym_type, syms->n * sizeof(SymbolType));

  /* reallocate the symbol name array. */
  syms->sym_name = (char**)
    realloc(syms->sym_name, syms->n * sizeof(char*));

  /* reallocate the symbol data array. */
  syms->sym_data = (void**)
    realloc(syms->sym_data, syms->n * sizeof(void*));

  /* check if reallocation failed. */
  if (!syms->sym_type || !syms->sym_name || !syms->sym_data)
    fail("unable to reallocate array");

  /* store the symbol type; initialize the symbol data. */
  syms->sym_type[syms->n - 1] = stype;
  syms->sym_data[syms->n - 1] = NULL;

  /* check if the symbol name is defined. */
  if (sname && !(stype & SYMBOL_TEMP)) {
    /* store the symbol name. */
    syms->sym_name[syms->n - 1] = strdup(sname);
  }
  else {
    /* generate a new symbol name. */
    char *sbuf = (char*) malloc(20 * sizeof(char));
    if (sbuf) snprintf(sbuf, 20, "_t%ld", syms->nt);

    /* store the symbol name. */
    syms->sym_name[syms->n - 1] = sbuf;
    syms->nt++;
  }

  /* check if symbol data should be stored. */
  if (stype & SYMBOL_INT) {
    /* allocate a data pointer. */
    long *data = (long*) malloc(sizeof(long));
    if (data) *data = data_int;

    /* set the integer data. */
    syms->sym_data[syms->n - 1] = (void*) data;
  }
  else if (stype & SYMBOL_FLOAT) {
    /* allocate a data pointer. */
    double *data = (double*) malloc(sizeof(double));
    if (data) *data = data_float;

    /* set the float data. */
    syms->sym_data[syms->n - 1] = (void*) data;
  }
  else if (stype & SYMBOL_COMPLEX) {
    /* allocate a data pointer. */
    complex double *data = (complex double*)
      malloc(sizeof(complex double));
    if (data) *data = data_complex;

    /* set the complex data. */
    syms->sym_data[syms->n - 1] = (void*) data;
  }
  else if (stype & SYMBOL_STRING) {
    /* set the string data. */
    syms->sym_data[syms->n - 1] = (void*) strdup(data_str);
  }

  /* return success. */
  return syms->n;
}

inline const char *symbols_get_name (Symbols syms, long index) {
  /* return the symbol name string. */
  return (syms && index >= 0 && index < syms->n ?
          syms->sym_name[index] : NULL);
}

/* symbols_get_int(): return the integer data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline long symbols_get_int (Symbols syms, long index) {
  /* return the symbol data as an integer. */
  return (syms && index >= 0 && index < syms->n && syms->sym_data[index] ?
          *((long*) syms->sym_data[index]) : 0L);
}

/* symbols_get_float(): return the float data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline double symbols_get_float (Symbols syms, long index) {
  /* return the symbol data as a float. */
  return (syms && index >= 0 && index < syms->n && syms->sym_data[index] ?
          *((double*) syms->sym_data[index]) : 0.0);
}

/* symbols_get_complex(): return the complex data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline complex double symbols_get_complex (Symbols syms, long index) {
  /* return the symbol data as a complex. */
  return (syms && index >= 0 && index < syms->n && syms->sym_data[index] ?
          *((complex double*) syms->sym_data[index]) : 0.0);
}

/* symbols_get_string(): return the string data of a symbol.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @index: zero-based symbol index in the table.
 */
inline const char *symbols_get_string (Symbols syms, long index) {
  /* return the symbol data as a string. */
  return (syms && index >= 0 && index < syms->n && syms->sym_data[index] ?
          (char*) syms->sym_data[index] : NULL);
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

