
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the complex float header. */
#include <matte/complex.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>

/* include headers for superior types. */
#include <matte/vector.h>
#include <matte/complex-vector.h>

/* complex_type(): return a pointer to the complex float object type.
 */
ObjectType complex_type (void) {
  /* return the struct address. */
  return &Complex_type;
}

/* complex_new(): allocate a new matte complex float.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated complex float.
 */
Complex complex_new (Zone z, Object args) {
  /* allocate a new complex float. */
  Complex f = (Complex) object_alloc(z, &Complex_type);
  if (!f)
    return NULL;

  /* initialize the float value. */
  f->value = 0.0;

  /* return the new float. */
  return f;
}

/* complex_new_with_value(): allocate a new matte complex float with a
 * set value.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @value: initial value of the float.
 *
 * returns:
 *  newly allocated and initialized complex float.
 */
Complex complex_new_with_value (Zone z, complex double value) {
  /* allocate a new complex float. */
  Complex f = complex_new(z, NULL);
  if (!f)
    return NULL;

  /* set the float value and return the new float. */
  f->value = value;
  return f;
}

/* complex_copy(): allocate a new matte complex float from another
 * matte complex float.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @f: matte complex float to duplicate.
 *
 * returns:
 *  duplicated matte complex float.
 */
Complex complex_copy (Zone z, Complex f) {
  /* return null if the input argument is null. */
  if (!f)
    return NULL;

  /* allocate a new complex float with the value of the input object. */
  return complex_new_with_value(z, f->value);
}

/* complex_copyconj(): allocate a new matte complex float that is the
 * complex conjugate of another matte complex float.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @f: matte complex float to conjugate.
 *
 * returns:
 *  conjugated duplicate matte complex float.
 */
Complex complex_copyconj (Zone z, Complex f) {
  /* allocate a new complex float. */
  Complex fnew = complex_new(z, NULL);
  if (!fnew)
    return NULL;

  /* store and return the conjugated input value. */
  fnew->value = conj(f->value);
  return fnew;
}

/* complex_get_value(): get the raw value of a matte complex float.
 *
 * arguments:
 *  @f: matte complex float to access.
 *
 * returns:
 *  complex double value of the complex float object.
 */
inline complex double complex_get_value (Complex f) {
  /* return the value of the complex float. */
  return (f ? f->value : 0.0);
}

/* complex_get_real(): get the raw real value of a matte complex float.
 *
 * arguments:
 *  @f: matte complex float to access.
 *
 * returns:
 *  double real-component value of the complex float object.
 */
inline double complex_get_real (Complex f) {
  /* return the real value of the complex float. */
  return (f ? creal(f->value) : 0.0);
}

/* complex_get_imag(): get the raw imaginary value of a matte complex float.
 *
 * arguments:
 *  @f: matte complex float to access.
 *
 * returns:
 *  double imaginary-component value of the complex float object.
 */
inline double complex_get_imag (Complex f) {
  /* return the imaginary value of the complex float. */
  return (f ? cimag(f->value) : 0.0);
}

/* complex_get_abs(): get the absolute value of a matte complex float.
 *
 * arguments:
 *  @f: matte complex float to access.
 *
 * returns:
 *  double float absolute value of the complex float object.
 */
inline double complex_get_abs (Complex f) {
  /* return the absolute value of the complex float. */
  return (f ? cabs(f->value) : 0.0);
}

/* complex_get_arg(): get the argument of a matte complex float.
 *
 * arguments:
 *  @f: matte complex float to access.
 *
 * returns:
 *  double float argument of the complex float object.
 */
inline double complex_get_arg (Complex f) {
  /* return the argument of the complex float. */
  return (f ? carg(f->value) : 0.0);
}

/* complex_set_value(): set the raw value of a matte complex float.
 *
 * arguments:
 *  @f: matte complex float to modify.
 *  @value: new value of the complex float.
 */
inline void complex_set_value (Complex f, complex double value) {
  /* return if the input is null. */
  if (!f)
    return;

  /* set the value of the complex float. */
  f->value = value;
}

/* complex_disp(): display function for complex floats.
 */
int complex_disp (Zone z, Complex f) {
  const double re = creal(f->value);
  const double im = cimag(f->value);

  printf("%lg %s %lgi\n", re,
         im < 0.0 ? "-" : "+",
         im < 0.0 ? -im : im);

  return 1;
}

/* complex_true(): assertion function for complex floats.
 */
int complex_true (Complex f) {
  return (creal(f->value) && cimag(f->value) ? 1 : 0);
}

/* complex_plus(): addition function for complex floats.
 */
Object complex_plus (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex + complex => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) +
                                  complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex + float => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) +
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex + int => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) +
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex + range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float + complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) float_get_value((Float) a) +
          complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int + complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) int_get_value((Int) a) +
          complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range + complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] += fval;

      return (Object) v;
    }
  }

  return NULL;
}

/* complex_minus(): subtraction function for complex floats.
 */
Object complex_minus (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex - complex => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) -
                                  complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex - float => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) -
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex - int => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) -
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex - range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = fval - v->data[i];

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float - complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) float_get_value((Float) a) -
          complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int - complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) int_get_value((Int) a) -
          complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range - complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] -= fval;

      return (Object) v;
    }
  }

  return NULL;
}

/* complex_uminus(): unary negation function for complex floats.
 */
Complex complex_uminus (Zone z, Complex a) {
  /* compute and return the negation. */
  return complex_new_with_value(z, -(a->value));
}

/* complex_times(): element-wise multiplication function for complex floats.
 */
Object complex_times (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex .* complex => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) *
                                  complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex .* float => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) *
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex .* int => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) *
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex .* range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] *= fval;

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float .* complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) float_get_value((Float) a) *
          complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int .* complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) int_get_value((Int) a) *
          complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range .* complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] *= fval;

      return (Object) v;
    }
  }

  return NULL;
}

/* complex_rdivide(): element-wise right-division function for complex floats.
 */
Object complex_rdivide (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex ./ complex => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) /
                                  complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex ./ float => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) /
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex ./ int => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) a) /
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex ./ range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = fval / v->data[i];

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float ./ complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) float_get_value((Float) a) /
          complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int ./ complex => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) int_get_value((Int) a) /
          complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range ./ complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] /= fval;

      return (Object) v;
    }
  }

  return NULL;
}

/* complex_ldivide(): element-wise left-division function for complex floats.
 */
Object complex_ldivide (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex .\ complex => complex */
      return (Object)
        complex_new_with_value(z, complex_get_value((Complex) b) /
                                  complex_get_value((Complex) a));
    }
    else if (IS_FLOAT(b)) {
      /* complex .\ float => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) float_get_value((Float) b) /
          complex_get_value((Complex) a));
    }
    else if (IS_INT(b)) {
      /* complex .\ int => complex */
      return (Object)
        complex_new_with_value(z,
          (complex double) int_get_value((Int) b) /
          complex_get_value((Complex) a));
    }
    else if (IS_RANGE(b)) {
      /* complex .\ range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] /= fval;

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float .\ complex => complex */
      return (Object)
        complex_new_with_value(z,
          complex_get_value((Complex) b) /
          (complex double) float_get_value((Float) a));
    }
    else if (IS_INT(a)) {
      /* int .\ complex => complex */
      return (Object)
        complex_new_with_value(z,
          complex_get_value((Complex) b) /
          (complex double) int_get_value((Int) a));
    }
    else if (IS_RANGE(a)) {
      /* range .\ complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = fval / v->data[i];

      return (Object) v;
    }
  }

  return NULL;
}

/* complex_power(): element-wise exponentiation function for complex floats.
 */
Object complex_power (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex .^ complex => complex */
      return (Object)
        complex_new_with_value(z, cpow(
          complex_get_value((Complex) a),
          complex_get_value((Complex) b)));
    }
    else if (IS_FLOAT(b)) {
      /* complex .^ float => complex */
      return (Object)
        complex_new_with_value(z, cpow(
          complex_get_value((Complex) a),
          (complex double) float_get_value((Float) b)));
    }
    else if (IS_INT(b)) {
      /* complex .^ int => complex */
      return (Object)
        complex_new_with_value(z, cpow(
          complex_get_value((Complex) a),
          (complex double) int_get_value((Int) b)));
    }
    else if (IS_RANGE(b)) {
      /* complex .^ range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = cpow(fval, v->data[i]);

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float .^ complex => complex */
      return (Object)
        complex_new_with_value(z, cpow(
          (complex double) float_get_value((Float) a),
          complex_get_value((Complex) b)));
    }
    else if (IS_INT(a)) {
      /* int .^ complex => complex */
      return (Object)
        complex_new_with_value(z, cpow(
          (complex double) int_get_value((Int) a),
          complex_get_value((Complex) b)));
    }
    else if (IS_RANGE(a)) {
      /* range .^ complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = cpow(v->data[i], fval);

      return (Object) v;
    }
  }

  return NULL;
}

/* complex_lt(): less-than comparison operation for complex floats. */
#define F lt
#define OP <
#include "complex-cmp.c"

/* complex_gt(): greater-than comparison operation for complex floats. */
#define F gt
#define OP >
#include "complex-cmp.c"

/* complex_le(): less-than or equal-to comparison operation
 * for complex floats.
 */
#define F le
#define OP <=
#include "complex-cmp.c"

/* complex_ge(): greater-than or equal-to comparison operation
 * for complex floats.
 */
#define F ge
#define OP >=
#include "complex-cmp.c"

/* complex_ne(): inequality operation for complex floats. */
#define F ne
#define OP !=
#include "complex-eq.c"

/* complex_eq(): equality operation for complex floats. */
#define F eq
#define OP ==
#include "complex-eq.c"

/* complex_and(): element-wise logical-and operation for complex floats. */
#define F and
#define OP &&
#include "complex-bool.c"

/* complex_or(): element-wise logical-or operation for complex floats. */
#define F or
#define OP ||
#include "complex-bool.c"

/* complex_mand(): matrix logical-and operation for complex floats.
 */
Int complex_mand (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex && complex => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) && complex_get_abs((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex && float => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) && float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex && int => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) && int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex && range => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) && range_all((Range) b));
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float && complex => int */
      return int_new_with_value(z,
        float_get_value((Float) a) && complex_get_abs((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int && complex => int */
      return int_new_with_value(z,
        int_get_value((Int) a) && complex_get_abs((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range && complex => int */
      return int_new_with_value(z,
        range_all((Range) a) && complex_get_abs((Complex) b));
    }
  }

  return NULL;
}

/* complex_mor(): matrix logical-or operation for complex floats.
 */
Int complex_mor (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex && complex => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) || complex_get_abs((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex && float => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) || float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex && int => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) || int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex && range => int */
      return int_new_with_value(z,
        complex_get_abs((Complex) a) || range_all((Range) b));
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float && complex => int */
      return int_new_with_value(z,
        float_get_value((Float) a) || complex_get_abs((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int && complex => int */
      return int_new_with_value(z,
        int_get_value((Int) a) || complex_get_abs((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range && complex => int */
      return int_new_with_value(z,
        range_all((Range) a) || complex_get_abs((Complex) b));
    }
  }

  return NULL;
}

/* complex_not(): logical negation operation for complex floats.
 */
Int complex_not (Zone z, Complex f) {
  return int_new_with_value(z, cabs(f->value) ? 0L : 1L);
}

/* complex_colon(): colon operation for complex floats.
 */
ComplexVector complex_colon (Zone z, Object a, Object b, Object c) {
  complex double begin, step, end;

  if (IS_COMPLEX(a))
    begin = complex_get_value((Complex) a);
  else if (IS_FLOAT(a))
    begin = (complex double) float_get_value((Float) a);
  else if (IS_INT(a))
    begin = (complex double) int_get_value((Int) a);
  else
    return NULL;

  if (IS_COMPLEX(b))
    step = complex_get_value((Complex) b);
  else if (IS_FLOAT(b))
    step = (complex double) float_get_value((Float) b);
  else if (IS_INT(b))
    step = (complex double) int_get_value((Int) b);
  else
    return NULL;

  if (IS_COMPLEX(c))
    end = complex_get_value((Complex) c);
  else if (IS_FLOAT(c))
    end = (complex double) float_get_value((Float) c);
  else if (IS_INT(c))
    end = (complex double) int_get_value((Int) c);
  else
    return NULL;

  long n = (long) ceil(creal((end - begin) / step)) + 1;
  if (n < 0)
    n = 0;

  ComplexVector x = complex_vector_new_with_length(z, n);
  if (!x)
    return NULL;

  for (long i = 0; i < n; i++, begin += step)
    x->data[i] = begin;

  return x;
}

/* float_horzcat(): horizontal concatenation function for complex floats.
 */
ComplexVector complex_horzcat (Zone z, int n, va_list vl) {
  ComplexVector x = complex_vector_new(z, NULL);
  if (!x)
    return NULL;

  for (long i = 0, ix = 0; i < n; i++) {
    Object obj = (Object) va_arg(vl, Object);

    if ((IS_COMPLEX(obj) || IS_FLOAT(obj) || IS_INT(obj)) &&
        !complex_vector_set_length(x, x->n + 1)) {
      object_free(z, x);
      return NULL;
    }

    if (IS_COMPLEX(obj)) {
      x->data[ix++] = complex_get_value((Complex) obj);
    }
    else if (IS_FLOAT(obj)) {
      x->data[ix++] = (complex double) float_get_value((Float) obj);
    }
    else if (IS_INT(obj)) {
      x->data[ix++] = (complex double) int_get_value((Int) obj);
    }
    else if (IS_RANGE(obj)) {
      Range r = (Range) obj;
      long nr = range_get_length(r);

      if (!complex_vector_set_length(x, x->n + nr)) {
        object_free(z, x);
        return NULL;
      }

      for (long elem = r->begin; elem <= r->end; elem += r->step)
        x->data[ix++] = (complex double) elem;
    }
    else {
      object_free(z, x);
      return NULL;
    }
  }

  return x;
}

/* complex_vertcat(): vertical concatenation function for complex floats.
 */
ComplexVector complex_vertcat (Zone z, int n, va_list vl) {
  ComplexVector x = complex_vector_new_with_length(z, n);
  if (!x)
    return NULL;

  for (long i = 0; i < x->n; i++) {
    Object obj = (Object) va_arg(vl, Object);

    if (IS_COMPLEX(obj)) {
      x->data[i] = complex_get_value((Complex) obj);
    }
    else if (IS_FLOAT(obj)) {
      x->data[i] = (complex double) float_get_value((Float) obj);
    }
    else if (IS_INT(obj)) {
      x->data[i] = (complex double) int_get_value((Int) obj);
    }
    else {
      object_free(z, x);
      return NULL;
    }
  }

  x->tr = CblasTrans;
  return x;
}

/* Complex_type: object type structure for matte complex floats.
 */
struct _ObjectType Complex_type = {
  "Complex",                                     /* name       */
  sizeof(struct _ComplexFloat),                  /* size       */
  4,                                             /* precedence */

  (obj_constructor) complex_new,                 /* fn_new    */
  (obj_constructor) complex_copy,                /* fn_copy   */
  NULL,                                          /* fn_delete */
  (obj_display)     complex_disp,                /* fn_disp   */
  (obj_assert)      complex_true,                /* fn_true   */

  (obj_binary)   complex_plus,                   /* fn_plus       */
  (obj_binary)   complex_minus,                  /* fn_minus      */
  (obj_unary)    complex_uminus,                 /* fn_uminus     */
  (obj_binary)   complex_times,                  /* fn_times      */
  (obj_binary)   complex_times,                  /* fn_mtimes     */
  (obj_binary)   complex_rdivide,                /* fn_rdivide    */
  (obj_binary)   complex_ldivide,                /* fn_ldivide    */
  (obj_binary)   complex_rdivide,                /* fn_mrdivide   */
  (obj_binary)   complex_ldivide,                /* fn_mldivide   */
  (obj_binary)   complex_power,                  /* fn_power      */
  (obj_binary)   complex_power,                  /* fn_mpower     */
  (obj_binary)   complex_lt,                     /* fn_lt         */
  (obj_binary)   complex_gt,                     /* fn_gt         */
  (obj_binary)   complex_le,                     /* fn_le         */
  (obj_binary)   complex_ge,                     /* fn_ge         */
  (obj_binary)   complex_ne,                     /* fn_ne         */
  (obj_binary)   complex_eq,                     /* fn_eq         */
  (obj_binary)   complex_and,                    /* fn_and        */
  (obj_binary)   complex_or,                     /* fn_or         */
  (obj_binary)   complex_mand,                   /* fn_mand       */
  (obj_binary)   complex_mor,                    /* fn_mor        */
  (obj_unary)    complex_not,                    /* fn_not        */
  (obj_ternary)  complex_colon,                  /* fn_colon      */
  (obj_unary)    complex_copyconj,               /* fn_ctranspose */
  (obj_unary)    complex_copy,                   /* fn_transpose  */
  (obj_variadic) complex_horzcat,                /* fn_horzcat    */
  (obj_variadic) complex_vertcat,                /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

