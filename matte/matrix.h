
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_MATRIX_H__
#define __MATTE_MATRIX_H__

/* include the object header. */
#include <matte/object.h>

/* IS_MATRIX: macro to check that an object is a matte matrix.
 */
#define IS_MATRIX(obj) \
  MATTE_TYPE_CHECK(obj, matrix_type())

/* Matrix: pointer to a struct _Matrix. */
typedef struct _Matrix *Matrix;
struct _ObjectType Matrix_type;

/* _Matrix: structure for holding a matrix of real numbers.
 */
struct _Matrix {
  /* base object. */
  OBJECT_BASE;

  /* @data: array of matrix elements.
   * @m: number of matrix rows, when untransposed.
   * @n: number of matrix columns, when untransposed.
   */
  double *data;
  long m, n;

  /* @tr: transposition status of the matrix.
   */
  int tr;
};

/* function declarations (matrix.c): */

ObjectType matrix_type (void);

Matrix matrix_new (Object args);

Matrix matrix_new_with_size (long m, long n);

Matrix matrix_copy (Matrix A);

void matrix_free (Matrix A);

long matrix_get_rows (Matrix A);

long matrix_get_cols (Matrix A);

int matrix_set_rows (Matrix A, long m);

int matrix_set_cols (Matrix A, long n);

#endif /* !__MATTE_MATRIX_H__ */

