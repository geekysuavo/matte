
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
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
  MatteTranspose tr;
};

/* function declarations (matrix.c): */

ObjectType matrix_type (void);

Matrix matrix_new (Zone z, Object args);

Matrix matrix_new_with_size (Zone z, long m, long n);

Matrix matrix_copy (Zone z, Matrix A);

void matrix_delete (Zone z, Matrix A);

long matrix_get_rows (Matrix A);

long matrix_get_cols (Matrix A);

long matrix_get_length (Matrix A);

double matrix_get (Matrix A, long i, long j);

double matrix_get_element (Matrix A, long i);

int matrix_set_rows (Matrix A, long m);

int matrix_set_cols (Matrix A, long n);

int matrix_set_size (Matrix A, long m, long n);

void matrix_set (Matrix A, long i, long j, double aij);

void matrix_set_element (Matrix A, long i, double ai);

#endif /* !__MATTE_MATRIX_H__ */

