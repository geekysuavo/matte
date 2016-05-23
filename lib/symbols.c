
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

  /* free the arrays of symbol names and types. */
  free(syms->sym_name);
  free(syms->sym_type);
}

/* symbols_find(): lookup a symbol in a matte symbol table.
 *
 * arguments:
 *  @syms: matte symbol table to access.
 *  @stype: symbol type to lookup.
 *  @sname: symbol name to lookup.
 *
 * returns:
 *  on successful symbol lookup, the one-based symbol index
 *  is returned. otherwise, zero (0) is returned.
 */
long symbols_find (Symbols syms, SymbolType stype, const char *sname) {
  /* return 'not found' if the either var is null. */
  if (!syms || !sname)
    return 0;

  /* search for the symbol in the array. */
  for (long i = 0; i < syms->n; i++) {
    /* on type and name match, return the one-based index. */
    if ((stype == SYMBOL_TYPE_ANY ||
         syms->sym_type[i] == stype) &&
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
 *
 * returns:
 *  on successful symbol addition/lookup, the one-based symbol index
 *  is returned. otherwise, zero (0) is returned.
 */
long symbols_add (Symbols syms, SymbolType stype, const char *sname) {
  /* validate the input arguments. */
  if (!syms)
    fail("invalid input arguments");

  /* if the symbol is already in the table, return its index. */
  long sid = symbols_find(syms, stype, sname);
  if (sid) return sid;

  /* increment the symbol table size. */
  syms->n++;

  /* reallocate the symbol type array. */
  syms->sym_type = (SymbolType*)
    realloc(syms->sym_type, syms->n * sizeof(SymbolType));

  /* reallocate the symbol name array. */
  syms->sym_name = (char**)
    realloc(syms->sym_name, syms->n * sizeof(char*));

  /* check if reallocation failed. */
  if (!syms->sym_type || !syms->sym_name)
    fail("unable to reallocate array");

  /* store the symbol type. */
  syms->sym_type[syms->n - 1] = stype;

  /* check if the symbol name is defined. */
  if (sname) {
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

  /* return success. */
  return syms->n;
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

