
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

/* function declarations (blas.c): */

void matte_dscal (double alpha, Vector x);

void matte_zscal (complex double alpha, ComplexVector x);

#endif /* !__MATTE_BLAS_H__ */

