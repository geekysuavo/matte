
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the complex vector header. */
#include <matte/complex-vector.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>
#include <matte/vector.h>

/* include headers for superior types. */
#include <matte/matrix.h>
#include <matte/complex-matrix.h>

/* complex_vector_type(): return a pointer to the complex vector object type.
 */
ObjectType complex_vector_type (void) {
  /* return the struct address. */
  return &ComplexVector_type;
}

/* complex_vector_new(): allocate a new empty matte complex vector.
 *
 * returns:
 *  newly allocated empty complex vector.
 */
ComplexVector complex_vector_new (Object args) {
  /* allocate a new complex vector. */
  ComplexVector x = (ComplexVector)
    ComplexVector_type.fn_alloc(&ComplexVector_type);

  /* check if allocation failed. */
  if (!x)
    return NULL;

  /* initialize the vector data and length. */
  x->data = NULL;
  x->n = 0;

  /* initialize the transposition state. */
  x->tr = CblasNoTrans;

  return x;
}

/* complex_vector_new_with_length(): allocate a new matte complex vector
 * with a set number of zero elements.
 *
 * arguments:
 *  @n: number of elements in the new vector.
 *
 * returns:
 *  newly allocated zero vector.
 */
ComplexVector complex_vector_new_with_length (long n) {
  /* allocate a new complex vector. */
  ComplexVector x = complex_vector_new(NULL);
  if (!x || !complex_vector_set_length(x, n))
    return NULL;

  /* return the newly allocated and initialized vector. */
  return x;
}

/* complex_vector_new_from_range(): allocate a new matte complex vector
 * from a matte range.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  newly allocated and initialized matte complex vector.
 */
ComplexVector complex_vector_new_from_range (Range r) {
  /* return null if the input range is null. */
  if (!r)
    return NULL;

  /* compute the length of the range. */
  long n = range_get_length(r);

  /* allocate a new complex vector. */
  ComplexVector x = complex_vector_new_with_length(n);
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
 *  @x: matte vector to access.
 *
 * returns:
 *  newly allocated and initialized complex vector.
 */
ComplexVector complex_vector_new_from_vector (Vector x) {
  /* return null if the input vector is null. */
  if (!x)
    return NULL;

  /* allocate a new complex vector. */
  ComplexVector y = complex_vector_new_with_length(x->n);
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
 *  @x: matte complex vector to duplicate.
 *
 * returns:
 *  duplicated matte complex vector.
 */
ComplexVector complex_vector_copy (ComplexVector x) {
  /* return null if the input argument is null. */
  if (!x)
    return NULL;

  /* allocate a new vector. */
  ComplexVector xnew = complex_vector_new(NULL);
  if (!xnew || !complex_vector_set_length(xnew, x->n))
    return NULL;

  /* copy the memory contents of the input vector into the duplicate. */
  if (xnew->n)
    memcpy(xnew->data, x->data, xnew->n * sizeof(complex double));

  /* copy the transposition state. */
  xnew->tr = x->tr;

  /* return the new complex vector. */
  return xnew;
}

/* complex_vector_free(): free all memory associated with a matte
 * complex vector.
 *
 * arguments:
 *  @x: matte complex vector to free.
 */
void complex_vector_free (ComplexVector x) {
  /* return if the vector is null. */
  if (!x)
    return;

  /* free the vector data. */
  if (x->data)
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
    fail("invalid input arguments");

  /* return if the vector already has the desired length. */
  if (x->n == n)
    return 1;

  /* reallocate the complex vector data. */
  x->data = (complex double*)
    realloc(x->data, n * sizeof(complex double));

  /* check if reallocation failed. */
  if (!x->data)
    fail("unable to reallocate array");

  /* fill the new trailing elements with zeros. */
  if (n > x->n)
    memset(x->data + x->n, 0, (n - x->n) * sizeof(complex double));

  /* store the new vector length. */
  x->n = n;

  /* return success. */
  return 1;
}

/* complex_vector_disp(): display function for matte complex vectors.
 */
int complex_vector_disp (ComplexVector x, const char *var) {
  printf("%s =\n", var);

  for (long i = 0; i < x->n; i++) {
    const double re = creal(x->data[i]);
    const double im = cimag(x->data[i]);
    printf("\n  %lg %s %lgi", re,
           im < 0.0 ? "-" : "+",
           im < 0.0 ? -im : im);
  }

  printf("\n\n");
  return 1;
}

/* complex_vector_ctranspose(): conjugate transposition function for
 * matte complex vectors.
 */
ComplexVector complex_vector_ctranspose (ComplexVector a) {
  ComplexVector atr = complex_vector_copy(a);
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
ComplexVector complex_vector_transpose (ComplexVector a) {
  ComplexVector atr = complex_vector_copy(a);
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

/* ComplexVector_type: object type structure for matte complex vectors.
 */
struct _ObjectType ComplexVector_type = {
  "ComplexVector",                               /* name       */
  sizeof(struct _ComplexVector),                 /* size       */
  6,                                             /* precedence */

  (obj_constructor) complex_vector_new,          /* fn_new     */
  (obj_allocator)   object_alloc,                /* fn_alloc   */
  (obj_destructor)  complex_vector_free,         /* fn_dealloc */
  (obj_display)     complex_vector_disp,         /* fn_disp    */

  NULL,                                          /* fn_plus       */
  NULL,                                          /* fn_minus      */
  NULL,                                          /* fn_uminus     */
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
  NULL                                           /* fn_subsindex  */
};

