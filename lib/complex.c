
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
int complex_disp (Zone z, Complex f, const char *var) {
  const double re = creal(f->value);
  const double im = cimag(f->value);

  printf("%s = %lg %s %lgi\n", var, re,
         im < 0.0 ? "-" : "+",
         im < 0.0 ? -im : im);

  return 1;
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

  (obj_binary)  complex_plus,                    /* fn_plus       */
  (obj_binary)  complex_minus,                   /* fn_minus      */
  (obj_unary)   complex_uminus,                  /* fn_uminus     */
  (obj_binary)  complex_times,                   /* fn_times      */
  NULL,                                          /* fn_mtimes     */
  (obj_binary)  complex_rdivide,                 /* fn_rdivide    */
  (obj_binary)  complex_ldivide,                 /* fn_ldivide    */
  NULL,                                          /* fn_mrdivide   */
  NULL,                                          /* fn_mldivide   */
  (obj_binary)  complex_power,                   /* fn_power      */
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
  (obj_unary)   complex_copyconj,                /* fn_ctranspose */
  (obj_unary)   complex_copy,                    /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

