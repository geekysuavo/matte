
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_COMPLEX_MATRIX_H__
#define __MATTE_COMPLEX_MATRIX_H__

/* include the object header. */
#include <matte/object.h>

/* IS_COMPLEX_MATRIX: macro to check that an object is a matte complex matrix.
 */
#define IS_COMPLEX_MATRIX(obj) \
  MATTE_TYPE_CHECK(obj, complex_matrix_type())

/* ComplexMatrix: pointer to a struct _ComplexMatrix. */
typedef struct _ComplexMatrix *ComplexMatrix;
struct _ObjectType ComplexMatrix_type;

/* _ComplexMatrix: structure for holding a matrix of complex numbers.
 */
struct _ComplexMatrix {
  /* base object. */
  OBJECT_BASE;

  /* @data: array of vector elements.
   * @m: number of matrix rows, when untransposed.
   * @n: number of matrix columns, when untransposed.
   */
  complex double *data;
  long m, n;

  /* @tr: transposition status of the matrix.
   */
  int tr;
};

/* function declarations (complex-matrix.c): */

ObjectType complex_matrix_type (void);

ComplexMatrix complex_matrix_new (Object args);

ComplexMatrix complex_matrix_new_with_size (long m, long n);

ComplexMatrix complex_matrix_copy (ComplexMatrix A);

void complex_matrix_free (ComplexMatrix A);

long complex_matrix_get_rows (ComplexMatrix A);

long complex_matrix_get_cols (ComplexMatrix A);

int complex_matrix_set_rows (ComplexMatrix A, long m);

int complex_matrix_set_cols (ComplexMatrix A, long n);

#endif /* !__MATTE_COMPLEX_MATRIX_H__ */

