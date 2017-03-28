
/* define macros for generating a range comparison method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(range_, name)

Vector FUNCTION(F) (Zone z, Object a, Object b) {
  if (IS_RANGE(a)) {
    if (IS_INT(b)) {
      /* range <op> int => vector */
      double fval = (double) int_get_value((Int) b);
      Vector v = vector_new_from_range(z, (Range) a);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        vector_set(v, i, vector_get(v, i) OP fval);

      return v;
    }
    else if (IS_RANGE(b)) {
      /* range <op> range => vector */
      if (range_get_length((Range) a) != range_get_length((Range) b))
        return NULL;

      Vector v = vector_new_from_range(z, (Range) a);
      Range r = (Range) b;
      if (!v)
        return NULL;

      for (long i = 0, elem = r->begin; i < v->n; i++, elem += r->step)
        vector_set(v, i, vector_get(v, i) OP (double) elem);

      return v;
    }
  }
  else if (IS_RANGE(b)) {
    if (IS_INT(a)) {
      /* int <op> range => vector */
      double fval = (double) int_get_value((Int) a);
      Vector v = vector_new_from_range(z, (Range) b);
      if (!v)
        return NULL;

      for (long i = 0; i < v->n; i++)
        vector_set(v, i, fval OP vector_get(v, i));

      return v;
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

