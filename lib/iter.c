
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the iterator and exception headers. */
#include <matte/iter.h>
#include <matte/except.h>

/* include headers for utilized types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>
#include <matte/vector.h>
#include <matte/matrix.h>
#include <matte/complex-vector.h>
#include <matte/complex-matrix.h>

/* next_int(): increment the value of an integer-based iterator.
 */
static int next_int (Zone z, Iter it) {
  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->val = (Object) int_copy(z, (Int) it->obj);
    return 1;
  }

  /* integers are valid for a single iteration. */
  return 0;
}

/* next_float(): increment the value of a float-based iterator.
 */
static int next_float (Zone z, Iter it) {
  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->val = (Object) float_copy(z, (Float) it->obj);
    return 1;
  }

  /* floats are valid for a single iteration. */
  return 0;
}

/* next_complex(): increment the value of a complex-based iterator.
 */
static int next_complex (Zone z, Iter it) {
  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->val = (Object) complex_copy(z, (Complex) it->obj);
    return 1;
  }

  /* complex floats are valid for a single iteration. */
  return 0;
}

/* next_range(): increment the value of a range-based iterator.
 */
static int next_range (Zone z, Iter it) {
  /* get the iteration object. */
  Range r = (Range) it->obj;

  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->i = range_get_begin(r);
    it->n = range_get_end(r);
    it->val = (Object) int_new_with_value(z, it->i);
  }
  else {
    /* update the iteration value. */
    it->i += range_get_step(r);
    int_set_value((Int) it->val, it->i);
  }

  /* check if the range limits were exceeded. */
  if (it->i > it->n)
    return 0;

  /* return for another iteration. */
  return 1;
}

/* next_vector(): increment the value of a vector-based iterator.
 */
static int next_vector (Zone z, Iter it) {
  /* get the iteration object. */
  Vector x = (Vector) it->obj;

  /* check if the vector has no elements. */
  if (!vector_get_length(x))
    return 0;

  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->i = 0;
    it->n = vector_get_length(x);
    it->val = (Object) float_new(z, NULL);
  }
  else {
    /* check if the vector has been exhausted. */
    if (++it->i >= it->n)
      return 0;
  }

  /* update the iteration value. */
  float_set_value((Float) it->val, vector_get(x, it->i));

  /* return for another iteration. */
  return 1;
}

/* next_complex_vector(): increment the value of a complex vector-based
 * iterator.
 */
static int next_complex_vector (Zone z, Iter it) {
  /* get the iteration object. */
  ComplexVector x = (ComplexVector) it->obj;

  /* check if the vector has no elements. */
  if (!complex_vector_get_length(x))
    return 0;

  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->i = 0;
    it->n = complex_vector_get_length(x);
    it->val = (Object) complex_new(z, NULL);
  }
  else {
    /* check if the vector has been exhausted. */
    if (++it->i >= it->n)
      return 0;
  }

  /* update the iteration value. */
  complex_set_value((Complex) it->val, complex_vector_get(x, it->i));

  /* return for another iteration. */
  return 1;
}

/* next_matrix(): increment the value of a matrix-based iterator.
 */
static int next_matrix (Zone z, Iter it) {
  /* get the iteration object. */
  Matrix A = (Matrix) it->obj;

  /* check if the matrix has no elements. */
  if (!matrix_get_length(A))
    return 0;

  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->i = 0;
    it->n = matrix_get_length(A);
    it->val = (Object) float_new(z, NULL);
  }
  else {
    /* check if the matrix has been exhausted. */
    if (++it->i >= it->n)
      return 0;
  }

  /* update the iteration value. */
  float_set_value((Float) it->val, matrix_get_element(A, it->i));

  /* return for another iteration. */
  return 1;
}

/* next_complex_matrix(): increment the value of a complex matrix-based
 * iterator.
 */
static int next_complex_matrix (Zone z, Iter it) {
  /* get the iteration object. */
  ComplexMatrix A = (ComplexMatrix) it->obj;

  /* check if the matrix has no elements. */
  if (!complex_matrix_get_length(A))
    return 0;

  /* check if initialization is required. */
  if (!it->val) {
    /* initialize the iteration value. */
    it->i = 0;
    it->n = complex_matrix_get_length(A);
    it->val = (Object) complex_new(z, NULL);
  }
  else {
    /* check if the matrix has been exhausted. */
    if (++it->i >= it->n)
      return 0;
  }

  /* update the iteration value. */
  complex_set_value((Complex) it->val,
    complex_matrix_get_element(A, it->i));

  /* return for another iteration. */
  return 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* iter_type(): return a pointer to the iterator object type.
 */
ObjectType iter_type (void) {
  /* return the struct address. */
  return &Iter_type;
}

/* iter_new(): allocate a new matte iterator and initialize
 * it from another matte object.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @obj: matte object to iterate over.
 *
 * returns:
 *  newly allocated and initialized iterator.
 */
Object iter_new (Zone z, Object obj) {
  /* raise an error if the master object is null. */
  if (!obj)
    throw(z, ERR_INVALID_ARGIN);

  /* allocate a new iterator. */
  Iter it = (Iter) object_alloc(z, &Iter_type);
  if (!it)
    return exceptions_get(z);

  /* store the master object. */
  it->obj = obj;

  /* initialize the iterator members for the first iteration. */
  it->val = NULL;
  it->i = it->n = -1L;

  /* return the new iterator. */
  return (Object) it;
}

/* iter_next(): increment the state of an iterator.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @it: matte iterator to modify.
 *
 * returns:
 *  whether or not the iterator has exhausted its data source.
 */
int iter_next (Zone z, Iter it) {
  /* get the type of the iteration expression. */
  const ObjectType type = MATTE_TYPE(it->obj);

  /* act based on type. */
  if (type == range_type())
    return next_range(z, it);
  else if (type == vector_type())
    return next_vector(z, it);
  else if (type == complex_vector_type())
    return next_complex_vector(z, it);
  else if (type == matrix_type())
    return next_matrix(z, it);
  else if (type == complex_matrix_type())
    return next_complex_matrix(z, it);
  else if (type == int_type())
    return next_int(z, it);
  else if (type == float_type())
    return next_float(z, it);
  else if (type == complex_type())
    return next_complex(z, it);

  /* unsupported type: throw an exception. */
  error(ERR_ITER_SUPPORT, type->name);
  it->val = exceptions_get(z);
  return 1;
}

/* iter_get_value(): get the current value of an iterator.
 *
 * arguments:
 *  @it: matte iterator to access.
 *
 * returns:
 *  current value of the iterator.
 */
inline Object iter_get_value (Iter it) {
  /* return the iteration value. */
  return it->val;
}

/* Iter_type: object type structure for matte iterators.
 */
struct _ObjectType Iter_type = {
  "Iter",                                        /* name       */
  sizeof(struct _Iter),                          /* size       */
  0,                                             /* precedence */

  NULL,                                          /* fn_new    */
  NULL,                                          /* fn_copy   */
  NULL,                                          /* fn_delete */
  NULL,                                          /* fn_disp   */
  NULL,                                          /* fn_true   */

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
  NULL,                                          /* fn_ctranspose */
  NULL,                                          /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};
