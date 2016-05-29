
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the matte header. */
#include <matte/matte.h>

/* include the compilation object headers. */
#include <matte/scanner.h>
#include <matte/parser.h>
#include <matte/ast.h>

/* object_alloc(): allocate a new matte object from its type structure.
 *
 * arguments:
 *  @z: zone allocator to request memory from, or NULL for malloc().
 *  @type: type structure to utilize for allocation.
 *
 * returns:
 *  newly allocated matte object.
 */
Object object_alloc (Zone z, ObjectType type) {
  /* return null if the type is null. */
  if (!type) {
    error(ERR_INVALID_ARGIN);
    return NULL;
  }

  /* allocate memory for the new object. */
  Object obj;
  if (z) {
    /* use the zone allocator. */
    obj = (Object) zone_alloc(z);
  }
  else {
    /* use standard malloc(). */
    obj = (Object) malloc(type->size);
  }

  /* check if allocation failed. */
  if (!obj) {
    error(ERR_OBJ_ALLOC, type->name);
    return NULL;
  }

  /* store the object type and return the new object. */
  obj->type = type;
  return obj;
}

/* object_free(): free all memory associated with a matte object.
 *
 * arguments:
 *  @z: zone allocator to release memory to, or NULL for free().
 *  @ptr: matte object pointer to free.
 */
void object_free (Zone z, void *ptr) {
  /* return if the pointer is null. */
  if (!ptr)
    return;

  /* obtain the object type. */
  Object obj = (Object) ptr;
  const ObjectType type = MATTE_TYPE(obj);

  /* return if the object is untyped or has no destructor. */
  if (!type || !type->fn_delete)
    return;

  /* call the destructor function and free the structure pointer. */
  type->fn_delete(z, obj);
  zone_free(z, obj);
}

/* object_free_all(): destruct and free all objects maintained by
 * a specified zone allocator.
 *
 * arguments:
 *  @z: zone allocator to release.
 */
void object_free_all (Zone z) {
  /* return if the zone pointer is null. */
  if (!z)
    return;

  /* compute the unit stride over the zone data, and initialize
   * a pointer into the zone data.
   */
  const unsigned long stride = z->usz;
  char *ptr = z->data;

  /* loop over all units of the zone. */
  for (unsigned long i = 0; i < z->n; i++, ptr += stride) {
    /* get the type of the current unit. */
    ObjectType type = MATTE_TYPE(ptr);

    /* if the type is valid and contains a destructor, execute it. */
    if (type && type->fn_delete)
      type->fn_delete(z, (Object) ptr);
  }

  /* destroy the zone. */
  zone_destroy(z);
}

/* object_disp(): display dispatch function.
 */
int object_disp (Zone z, Object obj, const char *var) {
  /* validate the input arguments. */
  if (!obj || !var)
    fail(ERR_INVALID_ARGIN);

  /* obtain the object type. */
  const ObjectType type = MATTE_TYPE(obj);

  /* return if the object is untyped or has no display function. */
  if (!type || !type->fn_disp)
    fail(ERR_OBJ_UNARY, "disp", type->name);

  /* return the result of the display function. */
  return type->fn_disp(z, obj, var);
}

/* object_plus(): addition dispatch function. */
#define F plus
#include "object-binary.c"

/* object_minus(): subtraction dispatch function. */
#define F minus
#include "object-binary.c"

/* object_uminus(): unary negation dispatch function. */
#define F uminus
#include "object-unary.c"

/* object_times(): element-wise multiplication dispatch function. */
#define F times
#include "object-binary.c"

/* object_mtimes(): matrix multiplication dispatch function. */
#define F mtimes
#include "object-binary.c"

/* object_rdivide(): element-wise right division dispatch function. */
#define F rdivide
#include "object-binary.c"

/* object_ldivide(): element-wise left division dispatch function. */
#define F ldivide
#include "object-binary.c"

/* object_mrdivide(): matrix right division dispatch function. */
#define F mrdivide
#include "object-binary.c"

/* object_mldivide(): matrix left division dispatch function. */
#define F mldivide
#include "object-binary.c"

/* object_power(): element-wise power dispatch function. */
#define F power
#include "object-binary.c"

/* object_mpower(): matrix power dispatch function. */
#define F mpower
#include "object-binary.c"

/* object_lt(): less than comparison dispatch function. */
#define F lt
#include "object-binary.c"

/* object_gt(): greater than comparison dispatch function. */
#define F gt
#include "object-binary.c"

/* object_le(): less than or equal to comparison dispatch function. */
#define F le
#include "object-binary.c"

/* object_ge(): greater than or equal to comparison dispatch function. */
#define F ge
#include "object-binary.c"

/* object_ne(): inequality comparison dispatch function. */
#define F ne
#include "object-binary.c"

/* object_eq(): equality comparison dispatch function. */
#define F eq
#include "object-binary.c"

/* object_and(): logical and dispatch function. */
#define F and
#include "object-binary.c"

/* object_or(): logical or dispatch function. */
#define F or
#include "object-binary.c"

/* object_mand(): matrix logical and dispatch function. */
#define F mand
#include "object-binary.c"

/* object_mor(): matrix logical or dispatch function. */
#define F mor
#include "object-binary.c"

/* object_not(): logical negation dispatch function. */
#define F not
#include "object-unary.c"

/* object_colon(): colon operator dispatch function. */
#define F colon
#include "object-ternary.c"

/* object_ctranspose(): conjugate transpose dispatch function. */
#define F ctranspose
#include "object-unary.c"

/* object_transpose(): matrix transpose dispatch function. */
#define F transpose
#include "object-unary.c"

/* object_horzcat(): horizontal concatenation dispatch function. */
#define F horzcat
#include "object-variadic.c"

/* object_vertcat(): vertical concatenation dispatch function. */
#define F vertcat
#include "object-variadic.c"

/* object_subsref(): subscripted reference dispatch function.
 */
Object object_subsref (Zone z, Object a, Object s) {
  if (!a || !s)
    throw(z, ERR_INVALID_ARGIN);

  const ObjectType ta = MATTE_TYPE(a);
  obj_binary fn = ta->fn_subsref;

  if (fn)
    return fn(z, a, s);

  throw(z, ERR_OBJ_UNARY, "subsref", ta->name);
}

/* object_subsasgn(): subscripted assignment dispatch function.
 */
Object object_subsasgn (Zone z, Object a, Object s, Object b) {
  if (!a || !s || !b)
    throw(z, ERR_INVALID_ARGIN);

  const ObjectType ta = MATTE_TYPE(a);
  obj_ternary fn = ta->fn_subsasgn;

  if (fn)
    return fn(z, a, s, b);

  throw(z, ERR_OBJ_UNARY, "subsasgn", ta->name);
}

/* object_subsindex(): subscript index dispatch function. */
#define F subsindex
#include "object-unary.c"

/* strdup(): duplicate a string.
 *
 * arguments:
 *  @string to duplicate.
 *
 * returns:
 *  pointer to the duplicated string, or null on failure.
 */
char *strdup (const char *s) {
  /* declare required variables:
   *  @sdup: duplicate string.
   *  @n: string length.
   */
  char *sdup;
  int n;

  /* return null if the input string is also null. */
  if (!s)
    return NULL;

  /* get the new string length. */
  n = strlen(s);

  /* allocate memory for the duplicate string. */
  sdup = (char*) malloc((n + 1) * sizeof(char));
  if (!sdup)
    return NULL;

  /* copy the string contents. */
  strcpy(sdup, s);
  sdup[n] = '\0';

  /* return the duplicate string. */
  return sdup;
}

