
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the vector, exception and blas headers. */
#include <matte/vector.h>
#include <matte/except.h>
#include <matte/blas.h>

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

  /* return the new vector. */
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

/* vector_get(): get an element from a matte vector.
 *
 * arguments:
 *  @x: matte vector to access.
 *  @i: element index to get.
 *
 * returns:
 *  requested vector element.
 */
inline double vector_get (Vector x, long i) {
  /* if the pointer and index are valid, return the element. */
  if (x && i < x->n)
    return x->data[i];

  /* return zero. */
  return 0.0;
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
    fail(ERR_INVALID_ARGIN);

  /* return if the vector already has the desired length. */
  if (x->n == n)
    return 1;

  /* reallocate the vector data. */
  x->data = (double*) realloc(x->data, n * sizeof(double));
  if (!x->data)
    fail(ERR_BAD_ALLOC);

  /* fill the new trailing elements with zeros. */
  const long excess = (n - x->n) * sizeof(double);
  if (n > x->n)
    memset(x->data + x->n, 0, excess);

  /* store the new vector length. */
  x->n = n;

  /* return success. */
  return 1;
}

/* vector_set(): set an element of a matte vector.
 *
 * arguments:
 *  @x: matte vector to modify.
 *  @i: element index to set.
 *  @xi: element value.
 */
inline void vector_set (Vector x, long i, double xi) {
  /* if the pointer and index are valid, set the element. */
  if (x && i < x->n)
    x->data[i] = xi;
}

/* vector_any(): inline short-circuit evaluator that any elements of
 * a vector are nonzero.
 *
 * arguments:
 *  @x: matte vector to access.
 *
 * returns:
 *  long integer indicating the result.
 */
inline long vector_any (Vector x) {
  /* loop over the elements of the vector to evaluate the result. */
  for (long i = 0; i < x->n; i++) {
    /* short-circuit to true if any element is nonzero. */
    if (vector_get(x, i)) return 1L;
  }

  /* return false. */
  return 0L;
}

/* vector_all(): inline short-circuit evaluator that all elements of
 * a vector are nonzero.
 *
 * arguments:
 *  @x: matte vector to access.
 *
 * returns:
 *  long integer indicating the result.
 */
inline long vector_all (Vector x) {
  /* loop over the elements of the vector to evaluate the test. */
  for (long i = 0; i < x->n; i++) {
    /* short-circuit to false if any element is zero. */
    if (!vector_get(x, i)) return 0L;
  }

  /* return true. */
  return 1L;
}

/* vector_add_const(): add a constant value to a matte vector.
 *
 * arguments:
 *  @x: matte vector to modify.
 *  @f: constant to add to @x.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int vector_add_const (Vector x, double f) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* add the constant to every element of the vector. */
  for (long i = 0; i < x->n; i++)
    x->data[i] += f;

  /* return success. */
  return 1;
}

/* vector_const_div(): divide by the elements a matte vector.
 *
 * arguments:
 *  @f: constant to divide by @x.
 *  @x: matte vector to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int vector_const_div (double f, Vector x) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* divide the constant by every element of the vector. */
  for (long i = 0; i < x->n; i++)
    x->data[i] = f / x->data[i];

  /* return success. */
  return 1;
}

/* vector_pow_const(): exponentiate the elements a matte vector.
 *
 * arguments:
 *  @x: matte vector to modify.
 *  @f: constant to exponentiate @x.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int vector_pow_const (Vector x, double f) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* raise every element of the vector to the constant power. */
  for (long i = 0; i < x->n; i++)
    x->data[i] = pow(x->data[i], f);

  /* return success. */
  return 1;
}

/* vector_const_pow(): exponentiate by the elements a matte vector.
 *
 * arguments:
 *  @f: constant to exponentiate by @x.
 *  @x: matte vector to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int vector_const_pow (double f, Vector x) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* raise the constant by every element of the vector. */
  for (long i = 0; i < x->n; i++)
    x->data[i] = pow(f, x->data[i]);

  /* return success. */
  return 1;
}

/* vector_negate(): negate the elements of a matte vector.
 *
 * arguments:
 *  @x: matte vector to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int vector_negate (Vector x) {
  /* fail if the vector is null. */
  if (!x)
    fail(ERR_INVALID_ARGIN);

  /* negate every element of the vector. */
  for (long i = 0; i < x->n; i++)
    x->data[i] = -(x->data[i]);

  /* return success. */
  return 1;
}

/* vector_disp(): display function for matte vectors.
 */
int vector_disp (Zone z, Vector x) {
  /* print the vector contents. */
  printf("\n");
  const long n = vector_get_length(x);
  for (long i = 0; i < n; i++)
    printf("\n  %lg", vector_get(x, i));

  /* print newlines and return success. */
  printf("\n\n");
  return 1;
}

/* vector_plus(): addition function for vectors.
 */
Object vector_plus (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      /* vector + vector => vector */
      Vector va = (Vector) a;
      Vector vb = (Vector) b;

      if (va->tr == vb->tr) {
        Vector vc = vector_copy(z, va);
        if (matte_daxpy(1.0, vb, vc))
          return (Object) vc;
      }
      else
        return (Object) matrix_new_from_vector_sum(z, 1.0, va, vb);
    }
    else if (IS_COMPLEX(b)) {
      /* vector + complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) a);
      if (complex_vector_add_const(v, fval))
        return (Object) v;
    }
    else if (IS_FLOAT(b)) {
      /* vector + float => vector */
      double fval = float_get_value((Float) b);
      Vector v = vector_copy(z, (Vector) a);
      if (vector_add_const(v, fval))
        return (Object) v;
    }
    else if (IS_INT(b)) {
      /* vector + int => vector */
      double fval = (double) int_get_value((Int) b);
      Vector v = vector_copy(z, (Vector) a);
      if (vector_add_const(v, fval))
        return (Object) v;
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex + vector => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) b);
      if (complex_vector_add_const(v, fval))
        return (Object) v;
    }
    else if (IS_FLOAT(a)) {
      /* float + vector => vector */
      double fval = float_get_value((Float) a);
      Vector v = vector_copy(z, (Vector) b);
      if (vector_add_const(v, fval))
        return (Object) v;
    }
    else if (IS_INT(a)) {
      /* int + vector => vector */
      double fval = (double) int_get_value((Int) a);
      Vector v = vector_copy(z, (Vector) b);
      if (vector_add_const(v, fval))
        return (Object) v;
    }
  }

  return NULL;
}

/* vector_minus(): subtraction function for vectors.
 */
Object vector_minus (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      /* vector - vector => vector */
      Vector va = (Vector) a;
      Vector vb = (Vector) b;

      if (va->tr == vb->tr) {
        Vector vc = vector_copy(z, va);
        if (matte_daxpy(-1.0, vb, vc))
          return (Object) vc;
      }
      else
        return (Object) matrix_new_from_vector_sum(z, -1.0, va, vb);
    }
    else if (IS_COMPLEX(b)) {
      /* vector - complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) a);
      if (complex_vector_add_const(v, -fval))
        return (Object) v;
    }
    else if (IS_FLOAT(b)) {
      /* vector - float => vector */
      double fval = float_get_value((Float) b);
      Vector v = vector_copy(z, (Vector) a);
      if (vector_add_const(v, -fval))
        return (Object) v;
    }
    else if (IS_INT(b)) {
      /* vector - int => vector */
      double fval = (double) int_get_value((Int) b);
      Vector v = vector_copy(z, (Vector) a);
      if (vector_add_const(v, -fval))
        return (Object) v;
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex - vector => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) b);
      if (complex_vector_add_const(v, -fval) &&
          complex_vector_negate(v))
        return (Object) v;
    }
    else if (IS_FLOAT(a)) {
      /* float - vector => vector */
      double fval = float_get_value((Float) a);
      Vector v = vector_copy(z, (Vector) b);
      if (vector_add_const(v, -fval) && vector_negate(v))
        return (Object) v;
    }
    else if (IS_INT(a)) {
      /* int - vector => vector */
      double fval = (double) int_get_value((Int) a);
      Vector v = vector_copy(z, (Vector) b);
      if (vector_add_const(v, -fval) && vector_negate(v))
        return (Object) v;
    }
  }

  return NULL;
}

/* vector_uminus(): unary negation function for vectors.
 */
Vector vector_uminus (Zone z, Vector a) {
  Vector aneg = vector_new_with_length(z, a->n);
  if (!matte_daxpy(-1.0, a, aneg))
    return NULL;

  aneg->tr = a->tr;
  return aneg;
}

/* vector_times(): element-wise multiplication function for vectors.
 */
Object vector_times (Zone z, Object a, Object b) {
  if (IS_VECTOR(a) && IS_VECTOR(b)) {
    /* vector .* vector */
    Vector va = (Vector) a;
    Vector vb = (Vector) b;

    if (va->n != vb->n)
      throw(z, ERR_SIZE_MISMATCH_VV(va, vb));

    Vector vc = vector_new_with_length(z, va->n);
    if (vc) {
      for (long i = 0; i < vc->n; i++)
        vector_set(vc, i, vector_get(va, i) * vector_get(vb, i));

      return (Object) vc;
    }

    return NULL;
  }

  /* properly cast the operands. */
  Vector x;
  Object s;
  if (IS_VECTOR(a)) {
    x = (Vector) a;
    s = b;
  }
  else {
    x = (Vector) b;
    s = a;
  }

  if (IS_COMPLEX(s)) {
    /* vector .* complex => complex vector */
    ComplexVector y = complex_vector_new_from_vector(z, x);
    complex double sval = complex_get_value((Complex) s);
    if (matte_zscal(sval, y))
      return (Object) y;
  }
  else if (IS_FLOAT(s)) {
    /* vector .* float => vector */
    Vector y = vector_copy(z, x);
    double sval = float_get_value((Float) s);
    if (matte_dscal(sval, y))
      return (Object) y;
  }
  else if (IS_INT(s)) {
    /* vector .* int => vector */
    Vector y = vector_copy(z, x);
    long sval = int_get_value((Int) s);
    if (matte_dscal((double) sval, y))
      return (Object) y;
  }

  return NULL;
}

/* vector_mtimes(): matrix multiplication function for vectors.
 */
Object vector_mtimes (Zone z, Object a, Object b) {
  if (IS_VECTOR(a) && IS_VECTOR(b)) {
    Vector va = (Vector) a;
    Vector vb = (Vector) b;

    if (va->tr == CblasNoTrans && vb->tr == CblasTrans) {
      /* column * row => matrix */
      Matrix A = matrix_new_with_size(z, va->n, vb->n);
      if (matte_dger(1.0, va, vb, A))
        return (Object) A;
    }
    else if (va->tr == CblasTrans && vb->tr == CblasNoTrans) {
      /* row * column => float */
      double fval;
      if (matte_ddot(va, vb, &fval))
        return (Object) float_new_with_value(z, fval);
    }

    throw(z, ERR_SIZE_MISMATCH_VV(va, vb));
  }

  return vector_times(z, a, b);
}

/* FIXME: implement vector division functions. */

/* vector_power(): element-wise exponentiation function for vectors.
 */
Object vector_power (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      Vector va = (Vector) a;
      Vector vb = (Vector) b;

      if (va->tr == vb->tr) {
        /* column .^ column => column
         * row    .^ row    => row
         */
        if (va->n != vb->n)
          throw(z, ERR_SIZE_MISMATCH_VV(va, vb));

        Vector vc = vector_new_with_length(z, va->n);
        vc->tr = va->tr;
        if (vc) {
          for (long i = 0; i < vc->n; i++)
            vector_set(vc, i, pow(vector_get(va, i), vector_get(vb, i)));

          return (Object) vc;
        }
      }
      else if (va->tr == CblasNoTrans) {
        /* column .^ row => matrix */
        Matrix A = matrix_new_with_size(z, va->n, vb->n);
        if (A) {
          for (long i = 0; i < A->m; i++)
            for (long j = 0; j < A->n; j++)
              matrix_set(A, i, j, pow(vector_get(va, i),
                                      vector_get(vb, j)));

          return (Object) A;
        }
      }
    }
    else if (IS_COMPLEX(b)) {
      /* vector .^ complex => complex vector */
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) a);
      complex double fval = complex_get_value((Complex) b);
      if (complex_vector_pow_const(v, fval))
        return (Object) v;
    }
    else if (IS_FLOAT(b)) {
      /* vector .^ float => vector */
      Vector v = vector_copy(z, (Vector) a);
      double fval = float_get_value((Float) b);
      if (vector_pow_const(v, fval))
        return (Object) v;
    }
    else if (IS_INT(b)) {
      /* vector .^ int => vector */
      Vector v = vector_copy(z, (Vector) a);
      double fval = int_get_value((Int) b);
      if (vector_pow_const(v, fval))
        return (Object) v;
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex .^ vector => complex vector */
      ComplexVector v = complex_vector_new_from_vector(z, (Vector) b);
      complex double fval = complex_get_value((Complex) a);
      if (complex_vector_const_pow(fval, v))
        return (Object) v;
    }
    else if (IS_FLOAT(a)) {
      /* float .^ vector => vector */
      Vector v = vector_copy(z, (Vector) b);
      double fval = float_get_value((Float) a);
      if (vector_const_pow(fval, v))
        return (Object) v;
    }
    else if (IS_INT(a)) {
      /* int .^ vector => vector */
      Vector v = vector_copy(z, (Vector) b);
      double fval = int_get_value((Int) a);
      if (vector_const_pow(fval, v))
        return (Object) v;
    }
  }

  return NULL;
}

/* vector_lt(): less-than comparison operation for vectors. */
#define F lt
#define OP <
#include "vector-cmp.c"

/* vector_gt(): greater-than comparison operation for vectors. */
#define F gt
#define OP >
#include "vector-cmp.c"

/* vector_le(): less-than or equal-to comparison operation for vectors. */
#define F le
#define OP <=
#include "vector-cmp.c"

/* vector_ge(): greater-than or equal-to comparison operation for vectors. */
#define F ge
#define OP >=
#include "vector-cmp.c"

/* vector_ne(): inequality comparison operation for vectors. */
#define CMPEQ
#define F ne
#define OP !=
#include "vector-cmp.c"

/* vector_eq(): equality comparison operation for vectors. */
#define CMPEQ
#define F eq
#define OP ==
#include "vector-cmp.c"

/* vector_and(): element-wise logical-and operation for vectors. */
#define F and
#define OP &&
#include "vector-cmp.c"

/* vector_or(): element-wise logical-or operation for vectors. */
#define F or
#define OP ||
#include "vector-cmp.c"

/* vector_mand(): matrix logical-and operation for vectors.
 */
Int vector_mand (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      /* vector && vector => int */
      return int_new_with_value(z,
        vector_all((Vector) a) && vector_all((Vector) b));
    }
    else if (IS_COMPLEX(b)) {
      /* vector && complex => int */
      return int_new_with_value(z,
        vector_all((Vector) a) && complex_get_abs((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* vector && float => int */
      return int_new_with_value(z,
        vector_all((Vector) a) && float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* vector && int => int */
      return int_new_with_value(z,
        vector_all((Vector) a) && int_get_value((Int) b));
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex && vector => int */
      return int_new_with_value(z,
         complex_get_abs((Complex) a) && vector_all((Vector) b));
    }
    else if (IS_FLOAT(a)) {
      /* float && vector => int */
      return int_new_with_value(z,
         float_get_value((Float) a) && vector_all((Vector) b));
    }
    else if (IS_INT(a)) {
      /* int && vector => int */
      return int_new_with_value(z,
         int_get_value((Int) a) && vector_all((Vector) b));
    }
  }

  return NULL;
}

/* vector_mor(): matrix logical-or operation for vectors.
 */
Int vector_mor (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      /* vector || vector => int */
      return int_new_with_value(z,
        vector_all((Vector) a) || vector_all((Vector) b));
    }
    else if (IS_COMPLEX(b)) {
      /* vector || complex => int */
      return int_new_with_value(z,
        vector_all((Vector) a) || complex_get_abs((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* vector || float => int */
      return int_new_with_value(z,
        vector_all((Vector) a) || float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* vector || int => int */
      return int_new_with_value(z,
        vector_all((Vector) a) || int_get_value((Int) b));
    }
  }
  else if (IS_VECTOR(b)) {
    if (IS_COMPLEX(a)) {
      /* complex || vector => int */
      return int_new_with_value(z,
         complex_get_abs((Complex) a) || vector_all((Vector) b));
    }
    else if (IS_FLOAT(a)) {
      /* float || vector => int */
      return int_new_with_value(z,
         float_get_value((Float) a) || vector_all((Vector) b));
    }
    else if (IS_INT(a)) {
      /* int || vector => int */
      return int_new_with_value(z,
         int_get_value((Int) a) || vector_all((Vector) b));
    }
  }

  return NULL;
}

/* vector_not(): logical negation operation for vectors.
 */
Vector vector_not (Zone z, Vector a) {
  Vector x = vector_copy(z, a);
  if (!x)
    return NULL;

  for (long i = 0; i < x->n; i++)
    vector_set(x, i, !vector_get(x, i));

  return x;
}

/* vector_transpose(): transposition function for vectors.
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
  NULL,                                          /* fn_true   */

  (obj_binary)   vector_plus,                    /* fn_plus       */
  (obj_binary)   vector_minus,                   /* fn_minus      */
  (obj_unary)    vector_uminus,                  /* fn_uminus     */
  (obj_binary)   vector_times,                   /* fn_times      */
  (obj_binary)   vector_mtimes,                  /* fn_mtimes     */
  NULL,                                          /* fn_rdivide    */
  NULL,                                          /* fn_ldivide    */
  NULL,                                          /* fn_mrdivide   */
  NULL,                                          /* fn_mldivide   */
  (obj_binary)   vector_power,                   /* fn_power      */
  NULL,                                          /* fn_mpower     */
  (obj_binary)   vector_lt,                      /* fn_lt         */
  (obj_binary)   vector_gt,                      /* fn_gt         */
  (obj_binary)   vector_le,                      /* fn_le         */
  (obj_binary)   vector_ge,                      /* fn_ge         */
  (obj_binary)   vector_ne,                      /* fn_ne         */
  (obj_binary)   vector_eq,                      /* fn_eq         */
  (obj_binary)   vector_and,                     /* fn_and        */
  (obj_binary)   vector_or,                      /* fn_or         */
  (obj_binary)   vector_mand,                    /* fn_mand       */
  (obj_binary)   vector_mor,                     /* fn_mor        */
  (obj_unary)    vector_not,                     /* fn_not        */
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

