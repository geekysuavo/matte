
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
 *  @type: type structure to utilize for allocation.
 *
 * returns:
 *  newly allocated matte object.
 */
Object object_alloc (ObjectType type) {
  /* declare required variables:
   *  @obj: object to allocate.
   */
  Object obj;

  /* return null if the type is null. */
  if (!type)
    throw("invalid object type");

  /* return null if the type has no size. */
  if (!type->size)
    throw("invalid object size");

  /* allocate memory for the new object. */
  obj = (Object) malloc(type->size);

  /* check if allocation failed. */
  if (!obj)
    throw("unable to allocate memory for %s", type->name);

  /* store the object type and return the new object. */
  obj->type = type;
  return obj;
}

/* object_new(): construct a new matte object.
 *
 * arguments:
 *  @type: type structure to utilize for allocation.
 *  @args: arguments passed to the constructor.
 *
 * returns:
 *  newly allocated and initialized matte object.
 */
Object object_new (ObjectType type, Object args) {
  /* return null if the type is null. */
  if (!type)
    return NULL;

  /* allocate a new matte object using the passed object type. */
  Object obj = type->fn_new(args);
  if (!obj)
    return NULL;

  /* return the allocated and initialized object. */
  return obj;
}

/* object_free(): free all memory associated with a matte object.
 *
 * arguments:
 *  @p: matte object to free.
 */
void object_free (Object obj) {
  /* return if the object is null. */
  if (!obj)
    return;

  /* obtain the object type. */
  const ObjectType type = MATTE_TYPE(obj);

  /* return if the object is untyped or has no destructor. */
  if (!type || !type->fn_dealloc)
    return;

  /* call the destructor function and free the structure pointer. */
  type->fn_dealloc(obj);
  free(obj);
}

/* object_disp(): display dispatch function.
 */
int object_disp (Object obj, const char *var) {
  /* validate the input arguments. */
  if (!obj || !var)
    fail("%s is undefined", var);

  /* obtain the object type. */
  const ObjectType type = MATTE_TYPE(obj);

  /* return if the object is untyped or has no display function. */
  if (!type || !type->fn_disp)
    fail("%s (type %s) has no display method", var, obj->type->name);

  /* return the result of the display function. */
  return type->fn_disp(obj, var);
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
Object object_subsref (Object a, Object s) {
  if (!a || !s)
    return NULL;

  const ObjectType ta = MATTE_TYPE(a);
  obj_binary fn = ta->fn_subsref;

  if (fn)
    return fn(a, s);

  return NULL;
}

/* object_subsasgn(): subscripted assignment dispatch function.
 */
Object object_subsasgn (Object a, Object s, Object b) {
  if (!a || !s || !b)
    return NULL;

  const ObjectType ta = MATTE_TYPE(a);
  obj_ternary fn = ta->fn_subsasgn;

  if (fn)
    return fn(a, s, b);

  return NULL;
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

