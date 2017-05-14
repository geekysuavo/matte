
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

/* matte_dcopy(): wrapper around cblas_dcopy().
 */
int matte_dcopy (Vector x, Vector y) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_dcopy(x->n, x->data, 1, y->data, 1);
  return 1;
}

/* matte_dswap(): wrapper around cblas_dswap().
 */
int matte_dswap (Vector x, Vector y) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_dswap(x->n, x->data, 1, y->data, 1);
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

/* matte_zcopy(): wrapper around cblas_zcopy().
 */
int matte_zcopy (ComplexVector x, ComplexVector y) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_zcopy(x->n, x->data, 1, y->data, 1);
  return 1;
}

/* matte_zswap(): wrapper around cblas_zswap().
 */
int matte_zswap (ComplexVector x, ComplexVector y) {
  /* fail if either vector is null. */
  if (!x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the vector lengths do not match. */
  if (x->n != y->n)
    fail(ERR_SIZE_MISMATCH_VV(x, y));

  /* execute the cblas function and return success. */
  cblas_zswap(x->n, x->data, 1, y->data, 1);
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

/* matte_dgemv(): wrapper around cblas_dgemv().
 */
int matte_dgemv (MatteTranspose trans, double alpha, Matrix A, Vector x,
                 double beta, Vector y) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
   fail(ERR_INVALID_ARGIN);

  /* fail if any sizes do not match. */
  if (trans == CblasNoTrans) {
    /* check the non-transposed sizes. */
    if (A->n != x->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, x));
    if (A->m != y->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, y));
  }
  else {
    /* check the transposed sizes. */
    if (A->m != x->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, x));
    if (A->n != y->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, y));
  }

  /* execute the cblas function. */
  cblas_dgemv(CblasColMajor, trans, A->m, A->n, alpha, A->data, A->m,
              x->data, 1, beta, y->data, 1);

  /* return success. */
  return 1;
}

/* matte_dsymv(): wrapper around cblas_dsymv().
 */
int matte_dsymv (MatteTriangle uplo, double alpha, Matrix A, Vector x,
                 double beta, Vector y) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the input matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the input vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* fail if the output vector size does not match. */
  if (A->m != y->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, y));

  /* execute the cblas function. */
  cblas_dsymv(CblasColMajor, uplo, A->m, alpha, A->data, A->m,
              x->data, 1, beta, y->data, 1);

  /* return success. */
  return 1;
}

/* matte_dtrmv(): wrapper around cblas_dtrmv().
 */
int matte_dtrmv (MatteTriangle uplo, MatteTranspose trans,
                 MatteDiagonal diag, Matrix A, Vector x) {
  /* fail if either pointer is null. */
  if (!A || !x)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the operand sizes does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(trans, A, x));

  /* execute the cblas function. */
  cblas_dtrmv(CblasColMajor, uplo, trans, diag,
              A->m, A->data, A->m, x->data, 1);

  /* return success. */
  return 1;
}

/* matte_dtrsv(): wrapper around cblas_dtrsv().
 */
int matte_dtrsv (MatteTriangle uplo, MatteTranspose trans,
                 MatteDiagonal diag, Matrix A, Vector x) {
  /* fail if either pointer is null. */
  if (!A || !x)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the operand sizes does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(trans, A, x));

  /* execute the cblas function. */
  cblas_dtrsv(CblasColMajor, uplo, trans, diag,
              A->m, A->data, A->m, x->data, 1);

  /* return success. */
  return 1;
}

/* matte_dger(): wrapper around cblas_dger().
 */
int matte_dger (double alpha, Vector x, Vector y, Matrix A) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the first vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* fail if the second vector size does not match. */
  if (A->n != y->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, y));

  /* execute the cblas function. */
  cblas_dger(CblasColMajor, A->m, A->n, alpha,
             x->data, 1, y->data, 1,
             A->data, A->m);

  /* return success. */
  return 1;
}

/* matte_dsyr(): wrapper around cblas_dsyr().
 */
int matte_dsyr (MatteTriangle uplo, double alpha, Vector x, Matrix A) {
  /* fail if either pointer is null. */
  if (!A || !x)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* execute the cblas function. */
  cblas_dsyr(CblasColMajor, uplo, A->m, alpha,
             x->data, 1, A->data, A->m);

  /* return success. */
  return 1;
}

/* matte_dsyr2(): wrapper around cblas_dsyr2().
 */
int matte_dsyr2 (MatteTriangle uplo, double alpha,
                 Vector x, Vector y, Matrix A) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the first vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* fail if the second vector size does not match. */
  if (A->m != y->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, y));

  /* execute the cblas function. */
  cblas_dsyr2(CblasColMajor, uplo, A->m, alpha,
              x->data, 1, y->data, 1,
              A->data, A->m);

  /* return success. */
  return 1;
}

/* === level 2, complex double === */

/* matte_zgemv(): wrapper around cblas_zgemv().
 */
int matte_zgemv (MatteTranspose trans, complex double alpha,
                 ComplexMatrix A, ComplexVector x,
                 complex double beta, ComplexVector y) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
   fail(ERR_INVALID_ARGIN);

  /* fail if any sizes do not match. */
  if (trans == CblasNoTrans) {
    /* check the non-transposed sizes. */
    if (A->n != x->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, x));
    if (A->m != y->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, y));
  }
  else {
    /* check the transposed sizes. */
    if (A->m != x->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, x));
    if (A->n != y->n) fail(ERR_SIZE_MISMATCH_MV(trans, A, y));
  }

  /* execute the cblas function. */
  cblas_zgemv(CblasColMajor, trans, A->m, A->n, &alpha, A->data, A->m,
              x->data, 1, &beta, y->data, 1);

  /* return success. */
  return 1;
}

/* matte_zhemv(): wrapper around cblas_zhemv().
 */
int matte_zhemv (MatteTriangle uplo, complex double alpha, ComplexMatrix A,
                 ComplexVector x, complex double beta, ComplexVector y) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the input matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the input vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* fail if the output vector size does not match. */
  if (A->m != y->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, y));

  /* execute the cblas function. */
  cblas_zhemv(CblasColMajor, uplo, A->m, &alpha, A->data, A->m,
              x->data, 1, &beta, y->data, 1);

  /* return success. */
  return 1;
}

/* matte_ztrmv(): wrapper around cblas_ztrmv().
 */
int matte_ztrmv (MatteTriangle uplo, MatteTranspose trans,
                 MatteDiagonal diag, ComplexMatrix A,
                 ComplexVector x) {
  /* fail if either pointer is null. */
  if (!A || !x)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the operand sizes does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(trans, A, x));

  /* execute the cblas function. */
  cblas_ztrmv(CblasColMajor, uplo, trans, diag,
              A->m, A->data, A->m, x->data, 1);

  /* return success. */
  return 1;
}

/* matte_ztrsv(): wrapper around cblas_ztrsv().
 */
int matte_ztrsv (MatteTriangle uplo, MatteTranspose trans,
                 MatteDiagonal diag, ComplexMatrix A,
                 ComplexVector x) {
  /* fail if either pointer is null. */
  if (!A || !x)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the operand sizes does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(trans, A, x));

  /* execute the cblas function. */
  cblas_ztrsv(CblasColMajor, uplo, trans, diag,
              A->m, A->data, A->m, x->data, 1);

  /* return success. */
  return 1;
}

/* matte_zher(): wrapper around cblas_zher().
 */
int matte_zher (MatteTriangle uplo, double alpha,
                ComplexVector x, ComplexMatrix A) {
  /* fail if either pointer is null. */
  if (!A || !x)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* execute the cblas function. */
  cblas_zher(CblasColMajor, uplo, A->m, alpha,
             x->data, 1, A->data, A->m);

  /* return success. */
  return 1;
}

/* matte_zher2(): wrapper around cblas_zher2().
 */
int matte_zher2 (MatteTriangle uplo, complex double alpha,
                 ComplexVector x, ComplexVector y,
                 ComplexMatrix A) {
  /* fail if any pointer is null. */
  if (!A || !x || !y)
    fail(ERR_INVALID_ARGIN);

  /* fail if the matrix is not square. */
  if (A->m != A->n)
    fail(ERR_SIZE_NONSQUARE(A));

  /* fail if the first vector size does not match. */
  if (A->m != x->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, x));

  /* fail if the second vector size does not match. */
  if (A->m != y->n)
    fail(ERR_SIZE_MISMATCH_MV(CblasNoTrans, A, y));

  /* execute the cblas function. */
  cblas_zher2(CblasColMajor, uplo, A->m, &alpha,
              x->data, 1, y->data, 1,
              A->data, A->m);

  /* return success. */
  return 1;
}

/* === level 3, double === */

/* matte_dgemm(): wrapper around cblas_dgemm().
 */
int matte_dgemm (MatteTranspose transA, MatteTranspose transB,
                 double alpha, Matrix A, Matrix B,
                 double beta, Matrix C) {
  /* fail if any pointer is null. */
  if (!A || !B || !C)
    fail(ERR_INVALID_ARGIN);

  /* store the output sizes. */
  const int m = C->m;
  const int n = C->n;
  const int k = (transA == CblasNoTrans ? A->n : A->m);

  /* fail if any first operand sizes do not match. */
  if (transA == CblasNoTrans) {
    if (A->m != m)
      fail(ERR_SIZE_MISMATCH_MM(transA, CblasNoTrans, A, C));
  }
  else {
    if (A->n != m)
      fail(ERR_SIZE_MISMATCH_MM(transA, CblasNoTrans, A, C));
  }

  /* fail if any second operand sizes do not match. */
  if (transB == CblasNoTrans) {
    if (B->m != k) fail(ERR_SIZE_MISMATCH_MM(transA, transB, A, B));
    if (B->n != n) fail(ERR_SIZE_MISMATCH_MM(transB, CblasNoTrans, B, C));
  }
  else {
    if (B->n != k) fail(ERR_SIZE_MISMATCH_MM(transA, transB, A, B));
    if (B->m != n) fail(ERR_SIZE_MISMATCH_MM(transB, CblasNoTrans, B, C));
  }

  /* execute the cblas function. */
  cblas_dgemm(CblasColMajor, transA, transB, m, n, k,
              alpha, A->data, A->m, B->data, B->m,
              beta, C->data, C->m);

  /* return success. */
  return 1;
}

/* FIXME: implement l3-d */

/* === level 3, complex double === */

/* matte_zgemm(): wrapper around cblas_zgemm().
 */
int matte_zgemm (MatteTranspose transA, MatteTranspose transB,
                 complex double alpha, ComplexMatrix A, ComplexMatrix B,
                 complex double beta, ComplexMatrix C) {
  /* fail if any pointer is null. */
  if (!A || !B || !C)
    fail(ERR_INVALID_ARGIN);

  /* store the output sizes. */
  const int m = C->m;
  const int n = C->n;
  const int k = (transA == CblasNoTrans ? A->n : A->m);

  /* fail if any first operand sizes do not match. */
  if (transA == CblasNoTrans) {
    if (A->m != m)
      fail(ERR_SIZE_MISMATCH_MM(transA, CblasNoTrans, A, C));
  }
  else {
    if (A->n != m)
      fail(ERR_SIZE_MISMATCH_MM(transA, CblasNoTrans, A, C));
  }

  /* fail if any second operand sizes do not match. */
  if (transB == CblasNoTrans) {
    if (B->m != k) fail(ERR_SIZE_MISMATCH_MM(transA, transB, A, B));
    if (B->n != n) fail(ERR_SIZE_MISMATCH_MM(transB, CblasNoTrans, B, C));
  }
  else {
    if (B->n != k) fail(ERR_SIZE_MISMATCH_MM(transA, transB, A, B));
    if (B->m != n) fail(ERR_SIZE_MISMATCH_MM(transB, CblasNoTrans, B, C));
  }

  /* execute the cblas function. */
  cblas_zgemm(CblasColMajor, transA, transB, m, n, k,
              &alpha, A->data, A->m, B->data, B->m,
              &beta, C->data, C->m);

  /* return success. */
  return 1;
}

/* FIXME: implement l3-z */

