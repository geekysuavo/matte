
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the complex vector, exception and blas headers. */
#include <matte/complex-vector.h>
#include <matte/except.h>
#include <matte/blas.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>
#include <matte/vector.h>
#include <matte/matrix.h>

/* include headers for superior types. */
#include <matte/complex-matrix.h>

/* complex_vector_type(): return a pointer to the complex vector object type.
 */
ObjectType complex_vector_type (void) {
  /* return the struct address. */
  return &ComplexVector_type;
}

/* complex_vector_new(): allocate a new empty matte complex vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty complex vector.
 */
ComplexVector complex_vector_new (Zone z, Object args) {
  /* allocate a new complex vector. */
  ComplexVector x = (ComplexVector) object_alloc(z, &ComplexVector_type);
  if (!x)
    return NULL;

  /* initialize the vector data and length. */
  x->data = NULL;
  x->n = 0;

  /* initialize the transposition state. */
  x->tr = CblasNoTrans;

  /* return the new vector. */
  return x;
}

/* complex_vector_new_with_length(): allocate a new matte complex vector
 * with a set number of zero elements.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @n: number of elements in the new vector.
 *
 * returns:
 *  newly allocated zero vector.
 */
ComplexVector complex_vector_new_with_length (Zone z, long n) {
  /* allocate a new complex vector. */
  ComplexVector x = complex_vector_new(z, NULL);
  if (!x || !complex_vector_set_length(x, n))
    return NULL;

  /* return the newly allocated and initialized vector. */
  return x;
}

/* complex_vector_new_from_range(): allocate a new matte complex vector
 * from a matte range.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @r: matte range to access.
 *
 * returns:
 *  newly allocated and initialized matte complex vector.
 */
ComplexVector complex_vector_new_from_range (Zone z, Range r) {
  /* return null if the input range is null. */
  if (!r)
    return NULL;

  /* compute the length of the range. */
  long n = range_get_length(r);

  /* allocate a new complex vector. */
  ComplexVector x = complex_vector_new_with_length(z, n);
  if (!x)
    return NULL;

  /* if any vector elements exist, compute and store them. */
  for (long i = 0, elem = r->begin; i < n; i++, elem += r->step)
    x->data[i] = (complex double) elem;

  /* return the new complex vector. */
  return x;
}

/* complex_vector_new_from_vector(): allocate a new matte complex vector
 * from a matte vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @x: matte vector to access.
 *
 * returns:
 *  newly allocated and initialized complex vector.
 */
ComplexVector complex_vector_new_from_vector (Zone z, Vector x) {
  /* return null if the input vector is null. */
  if (!x)
    return NULL;

  /* allocate a new complex vector. */
  ComplexVector y = complex_vector_new_with_length(z, x->n);
  if (!y)
    return NULL;

  /* copy the real data into the complex array. */
  for (long i = 0; i < x->n; i++)
    y->data[i] = (complex double) x->data[i];

  /* return new complex vector. */
  return y;
}

/* complex_vector_copy(): allocate a new matte complex vector from
 * another matte complex vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @x: matte complex vector to duplicate.
 *
 * returns:
 *  duplicated matte complex vector.
 */
ComplexVector complex_vector_copy (Zone z, ComplexVector x) {
  /* return null if the input argument is null. */
  if (!x)
    return NULL;

  /* allocate a new complex vector. */
  ComplexVector xnew = complex_vector_new(z, NULL);
  if (!xnew || !complex_vector_set_length(xnew, x->n))
    return NULL;

  /* copy the memory contents of the input vector into the duplicate. */
  if (xnew->n)
    memcpy(xnew->data, x->data, xnew->n * sizeof(complex double));

  /* copy the transposition state. */
  xnew->tr = x->tr;

  /* return the new vector. */
  return xnew;
}

/* complex_vector_delete(): free all memory associated with a matte
 * complex vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @x: matte complex vector to free.
 */
void complex_vector_delete (Zone z, ComplexVector x) {
  /* return if the vector is null. */
  if (!x)
    return;

  /* free the vector data. */
  free(x->data);
}

/* complex_vector_get_length(): get the length of a matte complex vector.
 *
 * arguments:
 *  @x: matte complex vector to access.
 *
 * returns:
 *  number of elements in the vector.
 */
long complex_vector_get_length (ComplexVector x) {
  /* return the vector length. */
  return (x ? x->n : 0);
}

/* complex_vector_get(): get an element from a matte complex vector.
 *
 * arguments:
 *  @x: matte complex vector to access.
 *  @i: element index to get.
 *
 * returns:
 *  requested vector element.
 */
inline complex double complex_vector_get (ComplexVector x, long i) {
  /* if the pointer and index are valid, return the element. */
  if (x && i < x->n)
    return x->data[i];

  /* return zero. */
  return 0.0;
}

/* complex_vector_set_length(): set the length of a matte complex vector.
 *
 * arguments:
 *  @x: matte complex vector to modify.
 *  @n: new length of the vector.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int complex_vector_set_length (ComplexVector x, long n) {
  /* validate the input arguments. */
  if (!x || n < 0)
    fail(ERR_INVALID_ARGIN);

  /* return if the vector already has the desired length. */
  if (x->n == n)
    return 1;

  /* reallocate the complex vector data. */
  x->data = (complex double*)
    realloc(x->data, n * sizeof(complex double));

  /* check if reallocation failed. */
  if (!x->data)
    fail(ERR_BAD_ALLOC);

  /* fill the new trailing elements with zeros. */
  const long excess = (n - x->n) * sizeof(complex double);
  if (n > x->n)
    memset(x->data + x->n, 0, excess);

  /* store the new vector length. */
  x->n = n;

  /* return success. */
  return 1;
}

/* complex_vector_set(): set an element of a matte complex vector.
 *
 * arguments:
 *  @x: matte complex vector to modify.
 *  @i: element index to set.
 *  @xi: element value.
 */
inline void complex_vector_set (ComplexVector x, long i,
                                complex double xi) {
  /* if the pointer and index are valid, set the element. */
  if (x && i < x->n)
    x->data[i] = xi;
}

/* complex_vector_add_const(): add a constant value to a matte
 * complex vector.
 *
 * arguments:
 *  @x: matte complex vector to modify.
 *  @f: constant to add to @x.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int complex_vector_add_const (ComplexVector x, complex double f) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* add the constant to every element of the vector. */
  for (long i = 0; i < x->n; i++)
    x->data[i] += f;

  /* return success. */
  return 1;
}

/* complex_vector_negate(): negate the elements of a matte complex vector.
 *
 * arguments:
 *  @x: matte complex vector to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int complex_vector_negate (ComplexVector x) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* negate every element of the vector. */
  for (long i = 0; i < x->n; i++)
    x->data[i] = -(x->data[i]);

  /* return success. */
  return 1;
}

/* complex_vector_disp(): display function for matte complex vectors.
 */
int complex_vector_disp (Zone z, ComplexVector x) {
  /* print the vector contents. */
  printf("\n");
  const long n = complex_vector_get_length(x);
  for (long i = 0; i < n; i++) {
    const complex double xi = complex_vector_get(x, i);
    const double re = creal(xi);
    const double im = cimag(xi);

    printf("\n  %lg %s %lgi", re,
           im < 0.0 ? "-" : "+",
           im < 0.0 ? -im : im);
  }

  /* print newlines and return success. */
  printf("\n\n");
  return 1;
}

/* complex_vector_ctranspose(): conjugate transposition function for
 * matte complex vectors.
 */
ComplexVector complex_vector_ctranspose (Zone z, ComplexVector a) {
  ComplexVector atr = complex_vector_copy(z, a);
  if (!atr)
    return NULL;

  switch (a->tr) {
    case CblasNoTrans: atr->tr = CblasConjTrans; break;
    case CblasTrans:
    case CblasConjTrans: atr->tr = CblasNoTrans; break;
    default: break;
  }

  return atr;
}

/* complex_vector_transpose(): matrix transposition function for
 * matte complex vectors.
 */
ComplexVector complex_vector_transpose (Zone z, ComplexVector a) {
  ComplexVector atr = complex_vector_copy(z, a);
  if (!atr)
    return NULL;

  switch (a->tr) {
    case CblasNoTrans: atr->tr = CblasTrans; break;
    case CblasTrans:
    case CblasConjTrans: atr->tr = CblasNoTrans; break;
    default: break;
  }

  return atr;
}

/* complex_vector_uminus(): unary negation function for matte
 * complex vectors.
 */
ComplexVector complex_vector_uminus (Zone z, ComplexVector a) {
  ComplexVector aneg = complex_vector_new_with_length(z, a->n);
  if (!matte_zaxpy(-1.0, a, aneg))
    return NULL;

  aneg->tr = a->tr;
  return aneg;
}

/* ComplexVector_type: object type structure for matte complex vectors.
 */
struct _ObjectType ComplexVector_type = {
  "ComplexVector",                               /* name       */
  sizeof(struct _ComplexVector),                 /* size       */
  7,                                             /* precedence */

  (obj_constructor) complex_vector_new,          /* fn_new    */
  (obj_constructor) complex_vector_copy,         /* fn_copy   */
  (obj_destructor)  complex_vector_delete,       /* fn_delete */
  (obj_display)     complex_vector_disp,         /* fn_disp   */
  NULL,                                          /* fn_true   */

  NULL,                                          /* fn_plus       */
  NULL,                                          /* fn_minus      */
  (obj_unary)    complex_vector_uminus,          /* fn_uminus     */
  NULL,                                          /* fn_times      */
  NULL,                                          /* fn_mtimes     */
  NULL,                                          /* fn_rdivide    */
  NULL,                                          /* fn_ldivide    */
  NULL,                                          /* fn_mrdivide   */
  NULL,                                          /* fn_mldivide   */
  NULL,                                          /* fn_power      */
  NULL,                                          /* fn_mpower     */
  NULL,                                          /* fn_lt         */
  NULL,                                          /* fn_gt         */
  NULL,                                          /* fn_le         */
  NULL,                                          /* fn_ge         */
  NULL,                                          /* fn_ne         */
  NULL,                                          /* fn_eq         */
  NULL,                                          /* fn_and        */
  NULL,                                          /* fn_or         */
  NULL,                                          /* fn_mand       */
  NULL,                                          /* fn_mor        */
  NULL,                                          /* fn_not        */
  NULL,                                          /* fn_colon      */
  (obj_unary)    complex_vector_ctranspose,      /* fn_ctranspose */
  (obj_unary)    complex_vector_transpose,       /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

