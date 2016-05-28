
/* define macros for generating a float comparison method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(float_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float <op> float => float */
      return (Object) float_new_with_value(z,
        float_get_value((Float) a) OP float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* float <op> int => float */
      return (Object) float_new_with_value(z,
        float_get_value((Float) a) OP (double) int_get_value((Int) b));
    }
    else if (IS_RANGE(b)) {
      /* float <op> range => vector */
      double fval = float_get_value((Float) a);
      Vector v = vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = (fval OP v->data[i]);

      return (Object) v;
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int <op> float => float */
      return (Object) float_new_with_value(z,
        (double) int_get_value((Int) a) OP float_get_value((Float) b));
    }
    else if (IS_RANGE(a)) {
      /* range <op> float => vector */
      double fval = float_get_value((Float) b);
      Vector v = vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        v->data[i] = (v->data[i] OP fval);

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

