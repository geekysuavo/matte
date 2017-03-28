
/* Copyright (c) 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the exception and blas headers. */
#include <matte/except.h>
#include <matte/blas.h>

/* === level 1, double === */

/* matte_daxpy(): wrapper around cblas_daxpy().
 */
int matte_daxpy (double alpha, Vector x, Vector y) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_daxpy(x->n, alpha, x->data, 1, y->data, 1);
  return 1;
}

/* matte_dscal(): wrapper around cblas_dscal().
 */
int matte_dscal (double alpha, Vector x) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* execute the cblas function and return success. */
  cblas_dscal(x->n, alpha, x->data, 1);
  return 1;
}

/* matte_ddot(): wrapper around cblas_ddot().
 */
int matte_ddot (Vector x, Vector y, double *out) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  *out = cblas_ddot(x->n, x->data, 1, y->data, 1);
  return 1;
}

/* matte_dnrm2(): wrapper around cblas_dnrm2().
 */
int matte_dnrm2 (Vector x, double *out) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* execute the cblas function and return success. */
  *out = cblas_dnrm2(x->n, x->data, 1);
  return 1;
}

/* === level 1, complex double === */

/* matte_zaxpy(): wrapper around cblas_zaxpy().
 */
int matte_zaxpy (complex double alpha, ComplexVector x, ComplexVector y) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_zaxpy(x->n, &alpha, x->data, 1, y->data, 1);
  return 1;
}

/* matte_zscal(): wrapper around cblas_zscal().
 */
int matte_zscal (complex double alpha, ComplexVector x) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* execute the cblas function and return success. */
  cblas_zscal(x->n, &alpha, x->data, 1);
  return 1;
}

/* matte_zdotu(): wrapper around cblas_zdotu().
 */
int matte_zdotu (ComplexVector x, ComplexVector y, complex double *out) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_zdotu_sub(x->n, x->data, 1, y->data, 1, out);
  return 1;
}

/* matte_zdotc(): wrapper around cblas_zdotc().
 */
int matte_zdotc (ComplexVector x, ComplexVector y, complex double *out) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_zdotc_sub(x->n, x->data, 1, y->data, 1, out);
  return 1;
}

/* matte_znrm2(): wrapper around cblas_znrm2().
 */
int matte_dznrm2 (ComplexVector x, double *out) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* execute the cblas function and return success. */
  *out = cblas_dznrm2(x->n, x->data, 1);
  return 1;
}

/* === level 2, double === */

/* FIXME */

/* === level 2, complex double === */

/* FIXME */

/* === level 3, double === */

/* FIXME */

/* === level 3, complex double === */

/* FIXME */

