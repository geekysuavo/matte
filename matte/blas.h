
/* Copyright (c) 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_BLAS_H__
#define __MATTE_BLAS_H__

/* include the vector and matrix headers. */
#include <matte/vector.h>
#include <matte/matrix.h>

/* include the complex vector and matrix headers. */
#include <matte/complex-vector.h>
#include <matte/complex-matrix.h>

/* include the atlas blas header. */
#include <cblas.h>

/* function declarations, level 1, double (blas.c): */

int matte_daxpy (double alpha, Vector x, Vector y);

int matte_dscal (double alpha, Vector x);

int matte_ddot (Vector x, Vector y, double *out);

int matte_dnrm2 (Vector x, double *out);

/* function declarations, level 1, complex double (blas.c): */

int matte_zaxpy (complex double alpha, ComplexVector x, ComplexVector y);

int matte_zscal (complex double alpha, ComplexVector x);

int matte_zdotu (ComplexVector x, ComplexVector y, complex double *out);

int matte_zdotc (ComplexVector x, ComplexVector y, complex double *out);

int matte_znrm2 (ComplexVector x, double *out);

/* function declarations, level 2, double (blas.c): */

/* FIXME */

/* function declarations, level 2, complex double (blas.c): */

/* FIXME */

/* function declarations, level 3, double (blas.c): */

/* FIXME */

/* function declarations, level 3, complex double (blas.c): */

/* FIXME */

#endif /* !__MATTE_BLAS_H__ */

