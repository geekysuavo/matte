
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_VECTOR_H__
#define __MATTE_VECTOR_H__

/* include the object and range headers. */
#include <matte/object.h>
#include <matte/range.h>

/* IS_VECTOR: macro to check that an object is a matte vector.
 */
#define IS_VECTOR(obj) \
  MATTE_TYPE_CHECK(obj, vector_type())

/* Vector: pointer to a struct _Vector. */
typedef struct _Vector *Vector;
struct _ObjectType Vector_type;

/* _Vector: structure for holding a vector of real numbers.
 */
struct _Vector {
  /* base object. */
  OBJECT_BASE;

  /* @data: array of vector elements.
   * @n: number of vector elements.
   */
  double *data;
  long n;

  /* @tr: transposition status of the vector.
   */
  MatteTranspose tr;
};

/* function declarations (vector.c): */

ObjectType vector_type (void);

Vector vector_new (Object args);

Vector vector_new_with_length (long n);

Vector vector_new_from_range (Range r);

Vector vector_copy (Vector x);

void vector_free (Vector x);

long vector_get_length (Vector x);

int vector_set_length (Vector x, long n);

#endif /* !__MATTE_VECTOR_H__ */

