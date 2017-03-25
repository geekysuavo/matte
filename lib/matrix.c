
/* Copyright (c) 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the matrix and exception headers. */
#include <matte/matrix.h>
#include <matte/except.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>
#include <matte/vector.h>

/* include headers for superior types. */
#include <matte/complex-vector.h>
#include <matte/complex-matrix.h>

/* matrix_type(): return a pointer to the matrix object type.
 */
ObjectType matrix_type (void) {
  /* return the struct address. */
  return &Matrix_type;
}

/* matrix_new(): allocate a new empty matte matrix.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty matrix.
 */
Matrix matrix_new (Zone z, Object args) {
  /* allocate a new matrix. */
  Matrix A = (Matrix) object_alloc(z, &Matrix_type);
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

/* matrix_new_with_size(): allocate a new matte matrix with a set
 * number of rows and columns, initialized to zero.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @m: number of rows in the new matrix.
 *  @n: number of columns in the new matrix.
 *
 * returns:
 *  newly allocated zero matrix.
 */
Matrix matrix_new_with_size (Zone z, long m, long n) {
  /* allocate a new matrix. */
  Matrix A = matrix_new(z, NULL);
  if (!A || !matrix_set_size(A, m, n))
    return NULL;

  /* return the newly allocated and initialized matrix. */
  return A;
}

/* matrix_copy(): allocate a new matte matrix from another matte matrix.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @A: matte matrix to duplicate.
 *
 * returns:
 *  duplicated matte matrix.
 */
Matrix matrix_copy (Zone z, Matrix A) {
  /* return null if the input argument is null. */
  if (!A)
    return NULL;

  /* allocate a new matrix. */
  Matrix Anew = matrix_new(z, NULL);
  if (!Anew || matrix_set_size(Anew, A->m, A->n))
    return NULL;

  /* copy the memory contents of the input matrix into the duplicate. */
  const long bytes = Anew->m * Anew->n * sizeof(double);
  if (bytes)
    memcpy(Anew->data, A->data, bytes);

  /* copy the transposition state. */
  Anew->tr = A->tr;

  /* return the new matrix. */
  return Anew;
}

/* matrix_delete(): free all memory associated with a matte matrix.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @A: matte matrix to free.
 */
void matrix_delete (Zone z, Matrix A) {
  /* return if the matrix is null. */
  if (!A)
    return;

  /* free the matrix data. */
  free(A->data);
}

/* matrix_get_rows(): get the row count of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to access.
 *
 * returns:
 *  number of rows in the matrix, when untransposed.
 */
long matrix_get_rows (Matrix A) {
  /* return the matrix row count. */
  return (A ? A->m : 0);
}

/* matrix_get_cols(): get the column count of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to access.
 *
 * returns:
 *  number of columns in the matrix, when untransposed.
 */
long matrix_get_cols (Matrix A) {
  /* return the matrix column count. */
  return (A ? A->n : 0);
}

/* matrix_get_length(): get the length of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to access.
 *
 * returns:
 *  number of elements in the matrix.
 */
long matrix_get_length (Matrix A) {
  /* return the matrix length. */
  return (A ? A->m * A->n : 0);
}

/* matrix_get(): get an element from a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to access.
 *  @i: element row index to get.
 *  @j: element column index to get.
 *
 * returns:
 *  requested matrix element.
 */
inline double matrix_get (Matrix A, long i, long j) {
  /* if the pointer and indices are valid, return the element. */
  if (A && i < A->m && j < A->n)
    return A->data[i + j * A->m];

  /* return zero. */
  return 0.0;
}

/* matrix_get_element(): get an element from a matte matrix by its
 * array index.
 *
 * arguments:
 *  @A: matte matrix to access.
 *  @i: element array index to get.
 *
 * returns:
 *  requested matrix element.
 */
inline double matrix_get_element (Matrix A, long i) {
  /* if the pointer and index are valid, return the element. */
  if (A && i < A->m * A->n)
    return A->data[i];

  /* return zero. */
  return 0.0;
}

/* matrix_set_rows(): set the row count of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to modify.
 *  @m: new row count of the matrix.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int matrix_set_rows (Matrix A, long m) {
  /* validate the input arguments. */
  if (!A || m < 0)
    fail(ERR_INVALID_ARGIN);

  /* set the new matrix dimensions. */
  return matrix_set_size(A, m, A->n);
}

/* matrix_set_cols(): set the column count of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to modify.
 *  @n: new column count of the matrix.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int matrix_set_cols (Matrix A, long n) {
  /* validate the input arguments. */
  if (!A || n < 0)
    fail(ERR_INVALID_ARGIN);

  /* set the new matrix dimensions. */
  return matrix_set_size(A, A->m, n);
}

/* matrix_set_size(): set the dimensions of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to modify.
 *  @m: new row count of the matrix.
 *  @n: new column count of the matrix.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int matrix_set_size (Matrix A, long m, long n) {
  /* validate the input arguments. */
  if (!A || m < 0 || n < 0)
    fail(ERR_INVALID_ARGIN);

  /* return if the matrix already has the desired dimensions. */
  if (A->m == m && A->n == n)
    return 1;

  /* compute the current and new allocation requirements. */
  const long bytes = A->m * A->n * sizeof(double);
  const long newbytes = m * n * sizeof(double);

  /* reallocate the matrix data. */
  A->data = (double*) realloc(A->data, newbytes);
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

/* matrix_set(): set an element of a matte matrix.
 *
 * arguments:
 *  @A: matte matrix to modify.
 *  @i: element row index to set.
 *  @j: element column index to set.
 *  @aij: element value.
 */
inline void matrix_set (Matrix A, long i, long j, double aij) {
  /* if the pointer and indices are valid, set the element. */
  if (A && i < A->m && j < A->n)
    A->data[i + j * A->m] = aij;
}

/* matrix_set_element(): set an element of a matte matrix by its
 * array index.
 *
 * arguments:
 *  @A: matte matrix to modify.
 *  @i: element array index to set.
 *  @ai: element value.
 */
inline void matrix_set_element (Matrix A, long i, double ai) {
  /* if the pointer and index are valid, set the element. */
  if (A && i < A->m * A->n)
    A->data[i] = ai;
}

/* matrix_disp(): display function for matte matrices.
 */
int matrix_disp (Zone z, Matrix A) {
  /* print the matrix contents. */
  printf("\n");
  for (long i = 0; i < A->m; i++) {
    printf("\n");
    for (long j = 0; j < A->n; j++)
      printf("  %lg", A->data[i * A->n + j]);
  }

  /* print newlines and return success. */
  printf("\n\n");
  return 1;
}

/* Matrix_type: object type structure for matte matrices.
 */
struct _ObjectType Matrix_type = {
  "Matrix",                                      /* name       */
  sizeof(struct _Matrix),                        /* size       */
  6,                                             /* precedence */

  (obj_constructor) matrix_new,                  /* fn_new    */
  (obj_constructor) matrix_copy,                 /* fn_copy   */
  (obj_destructor)  matrix_delete,               /* fn_delete */
  (obj_display)     matrix_disp,                 /* fn_disp   */
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

