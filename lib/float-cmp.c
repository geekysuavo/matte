
/* define macros for generating a float comparison method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(float_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float <op> float => int */
      return (Object) int_new_with_value(z,
        float_get_value((Float) a) OP float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* float <op> int => int */
      return (Object) int_new_with_value(z,
        float_get_value((Float) a) OP (double) int_get_value((Int) b));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int <op> float => int */
      return (Object) int_new_with_value(z,
        (double) int_get_value((Int) a) OP float_get_value((Float) b));
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

