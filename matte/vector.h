
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
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

Vector vector_new (Zone z, Object args);

Vector vector_new_with_length (Zone z, long n);

Vector vector_new_from_range (Zone z, Range r);

Vector vector_copy (Zone z, Vector x);

void vector_free (Zone z, Vector x);

long vector_get_length (Vector x);

double vector_get (Vector x, long i);

int vector_set_length (Vector x, long n);

void vector_set (Vector x, long i, double xi);

long vector_any (Vector x);

long vector_all (Vector x);

int vector_add_const (Vector x, double f);

int vector_const_div (double f, Vector x);

int vector_pow_const (Vector x, double f);

int vector_const_pow (double f, Vector x);

int vector_negate (Vector x);

#endif /* !__MATTE_VECTOR_H__ */

