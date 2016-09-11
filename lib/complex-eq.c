
/* define macros for generating a complex float comparison method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(complex_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    const complex double aval = complex_get_value((Complex) a);
    const double areal = creal(aval);
    const double aimag = cimag(aval);

    if (IS_COMPLEX(b)) {
      /* complex <op> complex => float */
      const complex double bval = complex_get_value((Complex) b);
      return (Object) float_new_with_value(z,
        areal OP creal(bval) && aimag OP cimag(bval));
    }
    else if (IS_FLOAT(b)) {
      /* complex <op> float => float */
      return (Object) float_new_with_value(z,
        areal OP float_get_value((Float) b) && aimag == 0.0);
    }
    else if (IS_INT(b)) {
      /* complex <op> int => float */
      return (Object) float_new_with_value(z,
        areal OP (double) int_get_value((Int) b) && aimag == 0.0);
    }
    else if (IS_RANGE(b)) {
      /* complex <op> range => vector */
      Vector v = vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = (areal OP v->data[i] && aimag == 0.0);

      return (Object) v;
    }
  }
  else if (IS_COMPLEX(b)) {
    const complex double bval = complex_get_value((Complex) b);
    const double breal = creal(bval);
    const double bimag = cimag(bval);

    if (IS_FLOAT(a)) {
      /* float <op> complex => float */
      return (Object) float_new_with_value(z,
        breal OP float_get_value((Float) a) && bimag == 0.0);
    }
    else if (IS_INT(a)) {
      /* int <op> complex => float */
      return (Object) float_new_with_value(z,
        breal OP (double) int_get_value((Int) a) && bimag == 0.0);
    }
    else if (IS_RANGE(a)) {
      /* range <op> complex => vector */
      Vector v = vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = (breal OP v->data[i] && bimag == 0.0);

      return (Object) v;
    }
  }

  return NULL;
}

/* undefine the method generator macros.
 */
#undef CONCAT
#undef FUNCTION
#undef OP
#undef F

