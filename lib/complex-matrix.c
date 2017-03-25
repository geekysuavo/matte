
/* Copyright (c) 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the complex matrix and exception headers. */
#include <matte/complex-matrix.h>
#include <matte/except.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>
#include <matte/vector.h>
#include <matte/matrix.h>
#include <matte/complex-vector.h>

/* complex_matrix_type(): return a pointer to the complex matrix object type.
 */
ObjectType complex_matrix_type (void) {
  /* return the struct address. */
  return &ComplexMatrix_type;
}

/* complex_matrix_new(): allocate a new empty matte complex matrix.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty complex matrix.
 */
ComplexMatrix complex_matrix_new (Zone z, Object args) {
  /* allocate a new complex matrix. */
  ComplexMatrix A = (ComplexMatrix) object_alloc(z, &ComplexMatrix_type);
  if (!A)
    return NULL;

  /* initialize the matrix data and size. */
  A->data = NULL;
  A->m = 0;
  A->n = 0;

  /* initialize the transposition status. */
  A->tr = CblasNoTrans;

  /* return the new matrix. */
  return A;
}

/* complex_matrix_new_with_size(): allocate a new matte complex matrix
 * with a set number of rows and columns, initialized to zero.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @m: number of rows in the new matrix.
 *  @n: number of columns in the new matrix.
 *
 * returns:
 *  newly allocated zero matrix.
 */
ComplexMatrix complex_matrix_new_with_size (Zone z, long m, long n) {
  /* allocate a new complex matrix. */
  ComplexMatrix A = complex_matrix_new(z, NULL);
  if (!A || !complex_matrix_set_size(A, m, n))
    return NULL;

  /* return the newly allocated and initialized matrix. */
  return A;
}

ComplexMatrix complex_matrix_new_from_matrix (Zone z, Matrix A) {
  /* FIXME: implement! */ return NULL;
}

/* complex_matrix_copy(): allocate a new matte complex matrix from
 * another matte complex matrix.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @A: matte complex matrix to duplicate.
 *
 * returns:
 *  duplicated matte complex matrix.
 */
ComplexMatrix complex_matrix_copy (Zone z, ComplexMatrix A) {
  /* return null if the input argument is null. */
  if (!A)
    return NULL;

  /* allocate a new complex matrix. */
  ComplexMatrix Anew = complex_matrix_new(z, NULL);
  if (!Anew || complex_matrix_set_size(Anew, A->m, A->n))
    return NULL;

  /* copy the memory contents of the input matrix into the duplicate. */
  const long bytes = Anew->m * Anew->n * sizeof(complex double);
  if (bytes)
    memcpy(Anew->data, A->data, bytes);

  /* copy the transposition state. */
  Anew->tr = A->tr;

  /* return the new matrix. */
  return Anew;
}

/* complex_matrix_delete(): free all memory associated with a matte
 * complex matrix.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @A: matte complex matrix to free.
 */
void complex_matrix_delete (Zone z, ComplexMatrix A) {
  /* return if the matrix is null. */
  if (!A)
    return;

  /* free the matrix data. */
  free(A->data);
}

/* complex_matrix_get_rows(): get the row count of a matte complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to access.
 *
 * returns:
 *  number of rows in the matrix, when untransposed.
 */
long complex_matrix_get_rows (ComplexMatrix A) {
  /* return the matrix row count. */
  return (A ? A->m : 0);
}

/* complex_matrix_get_cols(): get the column count of a matte
 * complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to access.
 *
 * returns:
 *  number of columns in the matrix, when untransposed.
 */
long complex_matrix_get_cols (ComplexMatrix A) {
  /* return the matrix column count. */
  return (A ? A->n : 0);
}

/* complex_matrix_get_length(): get the length of a matte complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to access.
 *
 * returns:
 *  number of elements in the matrix.
 */
long complex_matrix_get_length (ComplexMatrix A) {
  /* return the matrix length. */
  return (A ? A->m * A->n : 0);
}

/* complex_matrix_get(): get an element from a matte complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to access.
 *  @i: element row index to get.
 *  @j: element column index to get.
 *
 * returns:
 *  requested matrix element.
 */
inline complex double complex_matrix_get (ComplexMatrix A, long i, long j) {
  /* if the pointer and indices are valid, return the element. */
  if (A && i < A->m && j < A->n)
    return A->data[i + j * A->m];

  /* return zero. */
  return 0.0;
}

/* complex_matrix_get_element(): get an element from a matte complex
 * matrix by its array index.
 *
 * arguments:
 *  @A: matte complex matrix to access.
 *  @i: element array index to get.
 *
 * returns:
 *  requested matrix element.
 */
inline complex double complex_matrix_get_element (ComplexMatrix A, long i) {
  /* if the pointer and index are valid, return the element. */
  if (A && i < A->m * A->n)
    return A->data[i];

  /* return zero. */
  return 0.0;
}

/* complex_matrix_set_rows(): set the row count of a matte complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to modify.
 *  @m: new row count of the matrix.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int complex_matrix_set_rows (ComplexMatrix A, long m) {
  /* validate the input arguments. */
  if (!A || m < 0)
    fail(ERR_INVALID_ARGIN);

  /* set the new matrix dimensions. */
  return complex_matrix_set_size(A, m, A->n);
}

/* complex_matrix_set_cols(): set the column count of a matte
 * complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to modify.
 *  @n: new column count of the matrix.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int complex_matrix_set_cols (ComplexMatrix A, long n) {
  /* validate the input arguments. */
  if (!A || n < 0)
    fail(ERR_INVALID_ARGIN);

  /* set the new matrix dimensions. */
  return complex_matrix_set_size(A, A->m, n);
}

/* complex_matrix_set_size(): set the dimensions of a matte complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to modify.
 *  @m: new row count of the matrix.
 *  @n: new column count of the matrix.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int complex_matrix_set_size (ComplexMatrix A, long m, long n) {
  /* validate the input arguments. */
  if (!A || m < 0 || n < 0)
    fail(ERR_INVALID_ARGIN);

  /* return if the matrix already has the desired dimensions. */
  if (A->m == m && A->n == n)
    return 1;

  /* compute the current and new allocation requirements. */
  const long bytes = A->m * A->n * sizeof(complex double);
  const long newbytes = m * n * sizeof(complex double);

  /* reallocate the complex matrix data. */
  A->data = (complex double*) realloc(A->data, newbytes);
  if (!A->data)
    fail(ERR_BAD_ALLOC);

  /* fill the new trailing elements with zeros. */
  if (newbytes > bytes)
    memset(A->data + bytes, 0, newbytes - bytes);

  /* store the new matrix dimensions. */
  A->m = m;
  A->n = n;

  /* return success. */
  return 1;
}

/* complex_matrix_set(): set an element of a matte complex matrix.
 *
 * arguments:
 *  @A: matte complex matrix to modify.
 *  @i: element row index to set.
 *  @j: element column index to set.
 *  @aij: element value.
 */
inline void complex_matrix_set (ComplexMatrix A, long i, long j,
                                complex double aij) {
  /* if the pointer and indices are valid, set the element. */
  if (A && i < A->m && j < A->n)
    A->data[i + j * A->m] = aij;
}

/* complex_matrix_set_element(): set an element of a matte complex
 * matrix by its array index.
 *
 * arguments:
 *  @A: matte complex matrix to modify.
 *  @i: element array index to set.
 *  @ai: element value.
 */
inline void complex_matrix_set_element (ComplexMatrix A, long i,
                                        complex double ai) {
  /* if the pointer and index are valid, set the element. */
  if (A && i < A->m * A->n)
    A->data[i] = ai;
}

/* complex_matrix_disp(): display function for matte complex matrices.
 */
int complex_matrix_disp (Zone z, ComplexMatrix A) {
  /* print the matrix contents. */
  printf("\n");
  for (long i = 0; i < A->m; i++) {
    printf("\n");
    for (long j = 0; j < A->n; j++) {
      const double re = creal(A->data[i * A->n + j]);
      const double im = cimag(A->data[i * A->n + j]);
      printf("  %lg %s %lg", re,
             im < 0.0 ? "-" : "+",
             im < 0.0 ? -im : im);
    }
  }

  /* print newlines and return success. */
  printf("\n\n");
  return 1;
}

/* ComplexMatrix_type: object type structure for matte complex matrices.
 */
struct _ObjectType ComplexMatrix_type = {
  "ComplexMatrix",                               /* name       */
  sizeof(struct _ComplexMatrix),                 /* size       */
  6,                                             /* precedence */

  (obj_constructor) complex_matrix_new,          /* fn_new    */
  (obj_constructor) complex_matrix_copy,         /* fn_copy   */
  (obj_destructor)  complex_matrix_delete,       /* fn_delete */
  (obj_display)     complex_matrix_disp,         /* fn_disp   */
  NULL,                                          /* fn_true   */

  NULL,                                          /* fn_plus       */
  NULL,                                          /* fn_minus      */
  NULL,                                          /* fn_uminus     */
  NULL,                                          /* fn_times      */
  NULL,                                          /* fn_mtimes     */
  NULL,                                          /* fn_rdivide    */
  NULL,                                          /* fn_ldivide    */
  NULL,                                          /* fn_mrdivide   */
  NULL,                                          /* fn_mldivide   */
  NULL,                                          /* fn_power      */
  NULL,                                          /* fn_mpower     */
  NULL,                                          /* fn_lt         */
  NULL,                                          /* fn_gt         */
  NULL,                                          /* fn_le         */
  NULL,                                          /* fn_ge         */
  NULL,                                          /* fn_ne         */
  NULL,                                          /* fn_eq         */
  NULL,                                          /* fn_and        */
  NULL,                                          /* fn_or         */
  NULL,                                          /* fn_mand       */
  NULL,                                          /* fn_mor        */
  NULL,                                          /* fn_not        */
  NULL,                                          /* fn_colon      */
  NULL,                                          /* fn_ctranspose */
  NULL,                                          /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

