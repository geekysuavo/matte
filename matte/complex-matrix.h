
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
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
   * @m: number of matrix rows.
   * @n: number of matrix columns.
   */
  complex double *data;
  long m, n;
};

/* function declarations (complex-matrix.c): */

ObjectType complex_matrix_type (void);

ComplexMatrix complex_matrix_new (Zone z, Object args);

ComplexMatrix complex_matrix_new_with_size (Zone z, long m, long n);

ComplexMatrix complex_matrix_copy (Zone z, ComplexMatrix A);

ComplexMatrix complex_matrix_copy_trans (Zone z, ComplexMatrix A);

void complex_matrix_delete (Zone z, ComplexMatrix A);

long complex_matrix_get_rows (ComplexMatrix A);

long complex_matrix_get_cols (ComplexMatrix A);

long complex_matrix_get_length (ComplexMatrix A);

complex double complex_matrix_get (ComplexMatrix A, long i, long j);

complex double complex_matrix_get_element (ComplexMatrix A, long i);

int complex_matrix_set_rows (ComplexMatrix A, long m);

int complex_matrix_set_cols (ComplexMatrix A, long n);

int complex_matrix_set_size (ComplexMatrix A, long m, long n);

void complex_matrix_set (ComplexMatrix A, long i, long j,
                         complex double aij);

void complex_matrix_set_element (ComplexMatrix A, long i,
                                 complex double ai);

int complex_matrix_conj (ComplexMatrix A);

#endif /* !__MATTE_COMPLEX_MATRIX_H__ */

