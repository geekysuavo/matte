
/* Copyright (c) 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the blas header. */
#include <matte/blas.h>

/* matte_dscal(): wrapper around cblas_dscal().
 */
void matte_dscal (double alpha, Vector x) {
  /* return if the vector is null. */
  if (!x)
    return;

  /* execute the cblas function. */
  cblas_dscal(x->n, alpha, x->data, 1);
}

/* matte_zscal(): wrapper around cblas_zscal().
 */
void matte_zscal (complex double alpha, ComplexVector x) {
  /* return if the vector is null. */
  if (!x)
    return;

  /* execute the cblas function. */
  cblas_zscal(x->n, &alpha, x->data, 1);
}

