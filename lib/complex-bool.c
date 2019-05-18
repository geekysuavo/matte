
/* define macros for generating a complex float boolean method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(complex_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  if (IS_COMPLEX(a)) {
    if (IS_COMPLEX(b)) {
      /* complex <op> complex => int */
      return (Object) int_new_with_value(z,
        complex_get_abs((Complex) a) OP complex_get_abs((Complex) b));
    }
    else if (IS_FLOAT(b)) {
      /* complex <op> float => int */
      return (Object) int_new_with_value(z,
        complex_get_abs((Complex) a) OP float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* complex <op> int => int */
      return (Object) int_new_with_value(z,
        complex_get_abs((Complex) a) OP int_get_value((Int) b));
    }
  }
  else if (IS_COMPLEX(b)) {
    if (IS_FLOAT(a)) {
      /* float <op> complex => int */
      return (Object) int_new_with_value(z,
        float_get_value((Float) a) OP complex_get_abs((Complex) b));
    }
    else if (IS_INT(a)) {
      /* int <op> complex => int */
      return (Object) int_new_with_value(z,
        int_get_value((Int) a) OP complex_get_abs((Complex) b));
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

