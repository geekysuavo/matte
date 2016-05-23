
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
 * returns:
 *  newly allocated complex float.
 */
Complex complex_new (Object args) {
  /* allocate a new complex float. */
  Complex f = (Complex) Complex_type.fn_alloc(&Complex_type);
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
 *  @value: initial value of the float.
 *
 * returns:
 *  newly allocated and initialized complex float.
 */
Complex complex_new_with_value (complex double value) {
  /* allocate a new complex float. */
  Complex f = complex_new(NULL);
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
 *  @f: matte complex float to duplicate.
 *
 * returns:
 *  duplicated matte complex float.
 */
Complex complex_copy (Complex f) {
  /* return null if the input argument is null. */
  if (!f)
    return NULL;

  /* allocate a new complex float with the value of the input object. */
  Complex fnew = complex_new_with_value(complex_get_value(f));
  if (!fnew)
    return NULL;

  /* return the new complex float. */
  return fnew;
}

/* complex_copyconj(): allocate a new matte complex float that is the
 * complex conjugate of another matte complex float.
 *
 * arguments:
 *  @f: matte complex float to conjugate.
 *
 * returns:
 *  conjugated duplicate matte complex float.
 */
Complex complex_copyconj (Complex f) {
  /* copy the input value. */
  Complex fnew = complex_copy(f);
  if (!fnew)
    return NULL;

  /* conjugate the value and return it. */
  fnew->value = conj(fnew->value);
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
 * see Complex_type for more detailed information.
 */
int complex_disp (Complex f, const char *var) {
  printf("%s = %lg %s %lg\n",
         var, creal(f->value),
         cimag(f->value) < 0.0 ? "-" : "+",
         cimag(f->value));

  return 1;
}

/* complex_plus(): addition function for complex floats.
 * see Complex_type for more detailed information.
 */
Object complex_plus (Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex + complex => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) +
                               complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex + float => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) +
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex + int => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) +
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex + range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range((Range) b);
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
        complex_new_with_value((complex double) float_get_value((Float) a) +
                               complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int + complex => complex */
      return (Object)
        complex_new_with_value((complex double) int_get_value((Int) a) +
                               complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range + complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range((Range) a);
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
 * see Complex_type for more detailed information.
 */
Object complex_minus (Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex - complex => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) -
                               complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex - float => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) -
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex - int => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) -
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex - range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range((Range) b);
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
        complex_new_with_value((complex double) float_get_value((Float) a) -
                               complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int - complex => complex */
      return (Object)
        complex_new_with_value((complex double) int_get_value((Int) a) -
                               complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range - complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range((Range) a);
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
 * see Complex_type for more detailed information.
 */
Complex complex_uminus (Complex a) {
  /* compute and return the negation. */
  return complex_new_with_value(-complex_get_value(a));
}

/* complex_times(): element-wise multiplication function for complex floats.
 * see Complex_type for more detailed information.
 */
Object complex_times (Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex .* complex => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) *
                               complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex .* float => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) *
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex .* int => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) *
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex .* range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range((Range) b);
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
        complex_new_with_value((complex double) float_get_value((Float) a) *
                               complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int .* complex => complex */
      return (Object)
        complex_new_with_value((complex double) int_get_value((Int) a) *
                               complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range .* complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range((Range) a);
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
 * see Complex_type for more detailed information.
 */
Object complex_rdivide (Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex ./ complex => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) /
                               complex_get_value((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex ./ float => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) /
                               (complex double) float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex ./ int => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) a) /
                               (complex double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* complex ./ range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range((Range) b);
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
        complex_new_with_value((complex double) float_get_value((Float) a) /
                               complex_get_value((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int ./ complex => complex */
      return (Object)
        complex_new_with_value((complex double) int_get_value((Int) a) /
                               complex_get_value((Complex) b));
    }
    else if (IS_RANGE(a)) {
      /* range ./ complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range((Range) a);
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
 * see Complex_type for more detailed information.
 */
Object complex_ldivide (Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex .\ complex => complex */
      return (Object)
        complex_new_with_value(complex_get_value((Complex) b) /
                               complex_get_value((Complex) a));
    }
    else if (IS_FLOAT(b)) {
      /* complex .\ float => complex */
      return (Object)
        complex_new_with_value(
          (complex double) float_get_value((Float) b) /
          complex_get_value((Complex) a));
    }
    else if (IS_INT(b)) {
      /* complex .\ int => complex */
      return (Object)
        complex_new_with_value(
          (complex double) int_get_value((Int) b) /
          complex_get_value((Complex) a));
    }
    else if (IS_RANGE(b)) {
      /* complex .\ range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range((Range) b);
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
        complex_new_with_value(
          complex_get_value((Complex) b) /
          (complex double) float_get_value((Float) a));
    }
    else if (IS_INT(a)) {
      /* int .\ complex => complex */
      return (Object)
        complex_new_with_value(
          complex_get_value((Complex) b) /
          (complex double) int_get_value((Int) a));
    }
    else if (IS_RANGE(a)) {
      /* range .\ complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range((Range) a);
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
 * see Complex_type for more detailed information.
 */
Object complex_power (Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex .^ complex => complex */
      return (Object)
        complex_new_with_value(cpow(
          complex_get_value((Complex) a),
          complex_get_value((Complex) b)));
    }
    else if (IS_FLOAT(b)) {
      /* complex .^ float => complex */
      return (Object)
        complex_new_with_value(cpow(
          complex_get_value((Complex) a),
          (complex double) float_get_value((Float) b)));
    }
    else if (IS_INT(b)) {
      /* complex .^ int => complex */
      return (Object)
        complex_new_with_value(cpow(
          complex_get_value((Complex) a),
          (complex double) int_get_value((Int) b)));
    }
    else if (IS_RANGE(b)) {
      /* complex .^ range => complex vector */
      complex double fval = complex_get_value((Complex) a);
      ComplexVector v = complex_vector_new_from_range((Range) b);
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
        complex_new_with_value(cpow(
          (complex double) float_get_value((Float) a),
          complex_get_value((Complex) b)));
    }
    else if (IS_INT(a)) {
      /* int .^ complex => complex */
      return (Object)
        complex_new_with_value(cpow(
          (complex double) int_get_value((Int) a),
          complex_get_value((Complex) b)));
    }
    else if (IS_RANGE(a)) {
      /* range .^ complex => complex vector */
      complex double fval = complex_get_value((Complex) b);
      ComplexVector v = complex_vector_new_from_range((Range) a);
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
  "Complex",                           /* name       */
  sizeof(struct _ComplexFloat),        /* size       */
  4,                                   /* precedence */

  (obj_constructor) complex_new,       /* fn_new     */
  (obj_allocator)   object_alloc,      /* fn_alloc   */
  NULL,                                /* fn_dealloc */
  (obj_display) complex_disp,          /* fn_disp    */

  (obj_binary)  complex_plus,          /* fn_plus       */
  (obj_binary)  complex_minus,         /* fn_minus      */
  (obj_unary)   complex_uminus,        /* fn_uminus     */
  (obj_binary)  complex_times,         /* fn_times      */
  NULL,                                /* fn_mtimes     */
  (obj_binary)  complex_rdivide,       /* fn_rdivide    */
  (obj_binary)  complex_ldivide,       /* fn_ldivide    */
  NULL,                                /* fn_mrdivide   */
  NULL,                                /* fn_mldivide   */
  (obj_binary)  complex_power,         /* fn_power      */
  NULL,                                /* fn_mpower     */
  NULL,                                /* fn_lt         */
  NULL,                                /* fn_gt         */
  NULL,                                /* fn_le         */
  NULL,                                /* fn_ge         */
  NULL,                                /* fn_ne         */
  NULL,                                /* fn_eq         */
  NULL,                                /* fn_and        */
  NULL,                                /* fn_or         */
  NULL,                                /* fn_mand       */
  NULL,                                /* fn_mor        */
  NULL,                                /* fn_not        */
  NULL,                                /* fn_colon      */
  (obj_unary)   complex_copyconj,      /* fn_ctranspose */
  (obj_unary)   complex_copy,          /* fn_transpose  */
  NULL,                                /* fn_horzcat    */
  NULL,                                /* fn_vertcat    */
  NULL,                                /* fn_subsref    */
  NULL,                                /* fn_subsasgn   */
  NULL                                 /* fn_subsindex  */
};

