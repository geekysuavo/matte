
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_COMPLEX_VECTOR_H__
#define __MATTE_COMPLEX_VECTOR_H__

/* include the object and vector headers. */
#include <matte/object.h>
#include <matte/vector.h>

/* IS_COMPLEX_VECTOR: macro to check than an object is a matte complex vector.
 */
#define IS_COMPLEX_VECTOR(obj) \
  MATTE_TYPE_CHECK(obj, complex_vector_type())

/* ComplexVector: pointer to a struct _ComplexVector. */
typedef struct _ComplexVector *ComplexVector;
struct _ObjectType ComplexVector_type;

/* _ComplexVector: structure for holding a vector of complex numbers.
 */
struct _ComplexVector {
  /* base object. */
  OBJECT_BASE;

  /* @data: array of vector elements.
   * @n: number of vector elements.
   */
  complex double *data;
  long n;

  /* @tr: transposition status of the vector.
   */
  int tr;
};

/* function declarations (complex-vector.c): */

ObjectType complex_vector_type (void);

ComplexVector complex_vector_new (Zone z, Object args);

ComplexVector complex_vector_new_with_length (Zone z, long n);

ComplexVector complex_vector_new_from_range (Zone z, Range r);

ComplexVector complex_vector_new_from_vector (Zone z, Vector x);

ComplexVector complex_vector_copy (Zone z, ComplexVector x);

void complex_vector_free (Zone z, ComplexVector x);

long complex_vector_get_length (ComplexVector x);

complex double complex_vector_get (ComplexVector x, long i);

int complex_vector_set_length (ComplexVector x, long n);

void complex_vector_set (ComplexVector x, long i,
                         complex double xi);

void complex_vector_add_const (ComplexVector x, complex double f);

void complex_vector_negate (ComplexVector x);

#endif /* !__MATTE_COMPLEX_VECTOR_H__ */

