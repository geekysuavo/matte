
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the integer and exception headers. */
#include <matte/int.h>
#include <matte/except.h>

/* include headers for superior types. */
#include <matte/range.h>
#include <matte/vector.h>

/* int_type(): return a pointer to the integer object type.
 */
ObjectType int_type (void) {
  /* return the struct address. */
  return &Int_type;
}

/* int_new(): allocate a new matte integer.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated integer.
 */
Int int_new (Zone z, Object args) {
  /* allocate a new integer. */
  Int i = (Int) object_alloc(z, &Int_type);
  if (!i)
    return NULL;

  /* initialize the integer value. */
  i->value = 0L;

  /* return the new integer. */
  return i;
}

/* int_new_with_value(): allocate a new matte integer with a set value.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @value: initial value of the integer.
 *
 * returns:
 *  newly allocated and initialized integer.
 */
Int int_new_with_value (Zone z, long value) {
  /* allocate a new integer. */
  Int i = int_new(z, NULL);
  if (!i)
    return NULL;

  /* set the integer value and return the new integer. */
  i->value = value;
  return i;
}

/* int_copy(): allocate a new matte integer from another matte integer.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @i: matte integer to duplicate.
 *
 * returns:
 *  duplicated matte integer.
 */
Int int_copy (Zone z, Int i) {
  /* return null if the input argument is null. */
  if (!i)
    return NULL;

  /* allocate a new integer with the value of the input object. */
  Int inew = int_new_with_value(z, int_get_value(i));
  if (!inew)
    return NULL;

  /* return the new integer. */
  return inew;
}

/* int_get_value(): get the raw value of a matte integer.
 *
 * arguments:
 *  @i: matte integer to access.
 *
 * returns:
 *  long integer value of the int object.
 */
inline long int_get_value (Int i) {
  /* return the value of the integer. */
  return (i ? i->value : 0L);
}

/* int_set_value(): set the raw value of a matte integer.
 *
 * arguments:
 *  @i: matte integer to modify.
 *  @value: new value of the integer.
 */
inline void int_set_value (Int i, long value) {
  /* return if the input is null. */
  if (!i)
    return;

  /* set the value of the integer. */
  i->value = value;
}

/* int_disp(): display function for integers.
 */
int int_disp (Zone z, Int i, const char *var) {
  printf("%s = %ld\n", var, i->value);
  return 1;
}

/* int_plus(): addition function for integers.
 */
Int int_plus (Zone z, Int a, Int b) {
  /* compute and return the sum. */
  return int_new_with_value(z, a->value + b->value);
}

/* int_minus(): subtraction function for integers.
 */
Int int_minus (Zone z, Int a, Int b) {
  /* compute and return the difference. */
  return int_new_with_value(z, a->value - b->value);
}

/* int_uminus(): unary negation function for integers.
 */
Int int_uminus (Zone z, Int a) {
  /* compute and return the negation. */
  return int_new_with_value(z, -(a->value));
}

/* int_times(): multiplication function for integers.
 */
Int int_times (Zone z, Int a, Int b) {
  /* compute and return the product. */
  return int_new_with_value(z, a->value * b->value);
}

/* int_rdivide(): right-division function for integers.
 */
Int int_rdivide (Zone z, Int a, Int b) {
  /* compute and return the right-quotient. */
  return int_new_with_value(z, a->value / b->value);
}

/* int_ldivide(): left-division function for integers.
 */
Int int_ldivide (Zone z, Int a, Int b) {
  /* compute and return the left-quotient. */
  return int_new_with_value(z, b->value / a->value);
}

/* int_power(): exponentiation function for integers.
 */
Int int_power (Zone z, Int a, Int b) {
  /* obtain the values of the base and exponent. */
  long base = a->value;
  long exp =  b->value;

  /* check for trivial cases. */
  if (exp == 0L)
    return int_new_with_value(z, 1L);
  else if (exp < 0L || base == 0L)
    return int_new_with_value(z, 0L);

  /* loop until the exponent bits are exhausted. */
  long val = 1L;
  while (exp) {
    /* multiply on set bits. */
    if (exp & 1)
      val *= base;

    /* shift the exponent and base. */
    exp >>= 1;
    base *= base;
  }

  /* return the newly computed value. */
  return int_new_with_value(z, val);
}

/* int_lt(): less-than comparison function for integers.
 */
Int int_lt (Zone z, Int a, Int b) {
  /* return the comparison result. */
  return int_new_with_value(z, a->value < b->value);
}

/* int_gt(): greater-than comparison function for integers.
 */
Int int_gt (Zone z, Int a, Int b) {
  /* return the comparison result. */
  return int_new_with_value(z, a->value > b->value);
}

/* int_le(): less-than-or-equal-to comparison function for integers.
 */
Int int_le (Zone z, Int a, Int b) {
  /* return the comparison result. */
  return int_new_with_value(z, a->value <= b->value);
}

/* int_ge(): greater-than-or-equal-to comparison function for integers.
 */
Int int_ge (Zone z, Int a, Int b) {
  /* return the comparison result. */
  return int_new_with_value(z, a->value >= b->value);
}

/* int_ne(): inequality comparison function for integers.
 */
Int int_ne (Zone z, Int a, Int b) {
  /* return the comparison result. */
  return int_new_with_value(z, a->value != b->value);
}

/* int_eq(): equality comparison function for integers.
 */
Int int_eq (Zone z, Int a, Int b) {
  /* return the comparison result. */
  return int_new_with_value(z, a->value == b->value);
}

/* int_and(): logical-and operation for integers.
 */
Int int_and (Zone z, Int a, Int b) {
  /* return the logical result. */
  return int_new_with_value(z, a->value && b->value);
}

/* int_or(): logical-or operation for integers.
 */
Int int_or (Zone z, Int a, Int b) {
  /* return the logical result. */
  return int_new_with_value(z, a->value || b->value);
}

/* int_not(): logical negation operation for integers.
 */
Int int_not (Zone z, Int a) {
  /* return the logical result. */
  return int_new_with_value(z, a->value ? 0L : 1L);
}

/* int_colon(): colon operation for integers.
 */
Range int_colon (Zone z, Int a, Int b, Int c) {
  /* allocate a new range object. */
  Range r = range_new(z, NULL);

  /* set the values of the range. */
  range_set(r, a->value, b->value, c->value);

  /* return the new range. */
  return r;
}

/* int_horzcat(): horizontal concatenation function for integers.
 */
Vector int_horzcat (Zone z, int n, va_list vl) {
  Vector x = vector_new_with_length(z, n);
  if (!x)
    return NULL;

  for (long i = 0; i < x->n; i++) {
    Int iobj = (Int) va_arg(vl, Int);

    if (!IS_INT(iobj)) {
      object_free(z, x);
      return NULL;
    }

    x->data[i] = (double) iobj->value;
  }

  return x;
}

/* int_vertcat(): vertical concatenation function for integers.
 */
Vector int_vertcat (Zone z, int n, va_list vl) {
  Vector x = vector_new_with_length(z, n);
  if (!x)
    return NULL;

  for (long i = 0; i < x->n; i++) {
    Int iobj = (Int) va_arg(vl, Int);

    if (!IS_INT(iobj)) {
      object_free(z, x);
      return NULL;
    }

    x->data[i] = (double) iobj->value;
  }

  x->tr = CblasTrans;
  return x;
}

/* Int_type: object type structure for matte integers.
 */
struct _ObjectType Int_type = {
  "Int",                                         /* name       */
  sizeof(struct _Int),                           /* size       */
  1,                                             /* precedence */

  (obj_constructor) int_new,                     /* fn_new    */
  (obj_constructor) int_copy,                    /* fn_copy   */
  NULL,                                          /* fn_delete */
  (obj_display)     int_disp,                    /* fn_disp   */

  (obj_binary)   int_plus,                       /* fn_plus       */
  (obj_binary)   int_minus,                      /* fn_minus      */
  (obj_unary)    int_uminus,                     /* fn_uminus     */
  (obj_binary)   int_times,                      /* fn_times      */
  (obj_binary)   int_times,                      /* fn_mtimes     */
  (obj_binary)   int_rdivide,                    /* fn_rdivide    */
  (obj_binary)   int_ldivide,                    /* fn_ldivide    */
  (obj_binary)   int_rdivide,                    /* fn_mrdivide   */
  (obj_binary)   int_ldivide,                    /* fn_mldivide   */
  (obj_binary)   int_power,                      /* fn_power      */
  (obj_binary)   int_power,                      /* fn_mpower     */
  (obj_binary)   int_lt,                         /* fn_lt         */
  (obj_binary)   int_gt,                         /* fn_gt         */
  (obj_binary)   int_le,                         /* fn_le         */
  (obj_binary)   int_ge,                         /* fn_ge         */
  (obj_binary)   int_ne,                         /* fn_ne         */
  (obj_binary)   int_eq,                         /* fn_eq         */
  (obj_binary)   int_and,                        /* fn_and        */
  (obj_binary)   int_or,                         /* fn_or         */
  (obj_binary)   int_and,                        /* fn_mand       */
  (obj_binary)   int_or,                         /* fn_mor        */
  (obj_unary)    int_not,                        /* fn_not        */
  (obj_ternary)  int_colon,                      /* fn_colon      */
  (obj_unary)    int_copy,                       /* fn_ctranspose */
  (obj_unary)    int_copy,                       /* fn_transpose  */
  (obj_variadic) int_horzcat,                    /* fn_horzcat    */
  (obj_variadic) int_vertcat,                    /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

