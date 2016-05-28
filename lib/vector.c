
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the vector header. */
#include <matte/vector.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>

/* include headers for superior types. */
#include <matte/matrix.h>
#include <matte/complex-vector.h>
#include <matte/complex-matrix.h>

/* vector_type(): return a pointer to the vector object type.
 */
ObjectType vector_type (void) {
  /* return the struct address. */
  return &Vector_type;
}

/* vector_new(): allocate a new empty matte vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty vector.
 */
Vector vector_new (Zone z, Object args) {
  /* allocate a new vector. */
  Vector x = (Vector) object_alloc(z, &Vector_type);
  if (!x)
    return NULL;

  /* initialize the vector data and length. */
  x->data = NULL;
  x->n = 0;

  /* initialize the transposition state. */
  x->tr = CblasNoTrans;

  return x;
}

/* vector_new_with_length(): allocate a new matte vector with a set
 * number of zero elements.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @n: number of elements in the new vector.
 *
 * returns:
 *  newly allocated zero vector.
 */
Vector vector_new_with_length (Zone z, long n) {
  /* allocate a new vector. */
  Vector x = vector_new(z, NULL);
  if (!x || !vector_set_length(x, n))
    return NULL;

  /* return the newly allocated and initialized vector. */
  return x;
}

/* vector_new_from_range(): allocate a new matte vector using the contents
 * of a matte range.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @r: matte range to access.
 *
 * returns:
 *  newly allocated and initialized matte vector.
 */
Vector vector_new_from_range (Zone z, Range r) {
  /* return null if the input range is null. */
  if (!r)
    return NULL;

  /* compute the length of the range. */
  long n = range_get_length(r);

  /* allocate the vector. */
  Vector x = vector_new_with_length(z, n);
  if (!x)
    return NULL;

  /* if any vector elements exist, compute and store them. */
  for (long i = 0, elem = r->begin; i < n; i++, elem += r->step)
    x->data[i] = (double) elem;

  /* return the new vector. */
  return x;
}

/* vector_copy(): allocate a new matte vector from another matte vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @x: matte vector to duplicate.
 *
 * returns:
 *  duplicated matte vector.
 */
Vector vector_copy (Zone z, Vector x) {
  /* return null if the input argument is null. */
  if (!x)
    return NULL;

  /* allocate a new vector. */
  Vector xnew = vector_new(z, NULL);
  if (!xnew || !vector_set_length(xnew, x->n))
    return NULL;

  /* copy the memory contents of the input vector into the duplicate. */
  if (xnew->n)
    memcpy(xnew->data, x->data, xnew->n * sizeof(double));

  /* copy the transposition state. */
  xnew->tr = x->tr;

  /* return the new vector. */
  return xnew;
}

/* vector_delete(): free all memory associated with a matte vector.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @x: matte vector to free.
 */
void vector_delete (Zone z, Vector x) {
  /* return if the vector is null. */
  if (!x)
    return;

  /* free the vector data. */
  free(x->data);
}

/* vector_get_length(): get the length of a matte vector.
 *
 * arguments:
 *  @x: matte vector to access.
 *
 * returns:
 *  number of elements in the vector.
 */
long vector_get_length (Vector x) {
  /* return the vector length. */
  return (x ? x->n : 0);
}

/* vector_set_length(): set the length of a matte vector.
 *
 * arguments:
 *  @x: matte vector to modify.
 *  @n: new length of the vector.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int vector_set_length (Vector x, long n) {
  /* validate the input arguments. */
  if (!x || n < 0)
    fail("invalid input arguments");

  /* return if the vector already has the desired length. */
  if (x->n == n)
    return 1;

  /* reallocate the vector data. */
  x->data = (double*) realloc(x->data, n * sizeof(double));
  if (!x->data)
    fail("unable to reallocate array");

  /* fill the new trailing elements with zeros. */
  if (n > x->n)
    memset(x->data + x->n, 0, (n - x->n) * sizeof(double));

  /* store the new vector length. */
  x->n = n;

  /* return success. */
  return 1;
}

/* vector_disp(): display function for matte vectors.
 */
int vector_disp (Zone z, Vector x, const char *var) {
  printf("%s =\n", var);
  for (long i = 0; i < x->n; i++)
    printf("\n  %lg", x->data[i]);

  printf("\n\n");
  return 1;
}

/* vector_plus(): addition function for matte vectors.
 */
Object vector_plus (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      /* vector + vector => vector */
      Vector va = (Vector) a;
      Vector vb = (Vector) b;

      if (va->n != vb->n || va->tr != vb->tr)
        return NULL;

      Vector vc = vector_copy(z, va);
      if (!vc)
        return NULL;

      cblas_daxpy(vc->n, 1.0, vb->data, 1, va->data, 1);
      return (Object) vc;
    }
    else if (IS_COMPLEX(b)) {
      /* vector + complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
    else if (IS_FLOAT(b)) {
      /* vector + float => vector */
      double fval = float_get_value((Float) b);
      Vector v = vector_copy(z, (Vector) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
    else if (IS_INT(b)) {
      /* vector + int => vector */
      double fval = (double) int_get_value((Int) b);
      Vector v = vector_copy(z, (Vector) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex + vector => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
    else if (IS_FLOAT(a)) {
      /* float + vector => vector */
      double fval = float_get_value((Float) a);
      Vector v = vector_copy(z, (Vector) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
    else if (IS_INT(a)) {
      /* int + vector => vector */
      double fval = (double) int_get_value((Int) a);
      Vector v = vector_copy(z, (Vector) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
  }

  return NULL;
}

/* vector_minus(): subtraction function for matte vectors.
 */
Object vector_minus (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      /* vector - vector => vector */
      Vector va = (Vector) a;
      Vector vb = (Vector) b;

      if (va->n != vb->n || va->tr != vb->tr)
        return NULL;

      Vector vc = vector_copy(z, va);
      if (!vc)
        return NULL;

      cblas_daxpy(vc->n, -1.0, vb->data, 1, vc->data, 1);
      return (Object) vc;
    }
    else if (IS_COMPLEX(b)) {
      /* vector - complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] -= fval;

      return (Object) v;
    }
    else if (IS_FLOAT(b)) {
      /* vector - float => vector */
      double fval = float_get_value((Float) b);
      Vector v = vector_copy(z, (Vector) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] -= fval;

      return (Object) v;
    }
    else if (IS_INT(b)) {
      /* vector - int => vector */
      double fval = (double) int_get_value((Int) b);
      Vector v = vector_copy(z, (Vector) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] -= fval;

      return (Object) v;
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex - vector => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = fval - v->data[i];

      return (Object) v;
    }
    else if (IS_FLOAT(a)) {
      /* float - vector => vector */
      double fval = float_get_value((Float) a);
      Vector v = vector_copy(z, (Vector) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = fval - v->data[i];

      return (Object) v;
    }
    else if (IS_INT(a)) {
      /* int - vector => vector */
      double fval = (double) int_get_value((Int) a);
      Vector v = vector_copy(z, (Vector) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = fval - v->data[i];

      return (Object) v;
    }
  }

  return NULL;
}

/* vector_uminus(): unary negation function for matte vectors.
 */
Vector vector_uminus (Zone z, Vector a) {
  Vector aneg = vector_new_with_length(z, a->n);
  if (!aneg)
    return NULL;

  cblas_daxpy(aneg->n, -1.0, a->data, 1, aneg->data, 1);
  aneg->tr = a->tr;

  return aneg;
}

/* vector_transpose(): transposition function for matte vectors.
 */
Vector vector_transpose (Zone z, Vector a) {
  Vector atr = vector_copy(z, a);
  if (!atr)
    return NULL;

  switch (a->tr) {
    case CblasNoTrans: atr->tr = CblasTrans; break;
    case CblasTrans:
    case CblasConjTrans: atr->tr = CblasNoTrans; break;
    default: break;
  }

  return atr;
}

/* Vector_type: object type structure for matte vectors.
 */
struct _ObjectType Vector_type = {
  "Vector",                                      /* name       */
  sizeof(struct _Vector),                        /* size       */
  5,                                             /* precedence */

  (obj_constructor) vector_new,                  /* fn_new    */
  (obj_constructor) vector_copy,                 /* fn_copy   */
  (obj_destructor)  vector_delete,               /* fn_delete */
  (obj_display)     vector_disp,                 /* fn_disp   */

  (obj_binary)   vector_plus,                    /* fn_plus       */
  (obj_binary)   vector_minus,                   /* fn_minus      */
  (obj_unary)    vector_uminus,                  /* fn_uminus     */
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
  (obj_unary)    vector_transpose,               /* fn_ctranspose */
  (obj_unary)    vector_transpose,               /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

