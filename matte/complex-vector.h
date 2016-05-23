
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
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

ComplexVector complex_vector_new (Object args);

ComplexVector complex_vector_new_with_length (long n);

ComplexVector complex_vector_new_from_range (Range r);

ComplexVector complex_vector_new_from_vector (Vector x);

ComplexVector complex_vector_copy (ComplexVector x);

void complex_vector_free (ComplexVector x);

long complex_vector_get_length (ComplexVector x);

int complex_vector_set_length (ComplexVector x, long n);

#endif /* !__MATTE_COMPLEX_VECTOR_H__ */

