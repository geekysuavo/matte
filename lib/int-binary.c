
/* define macros for generating an integer binary method.
 */
#define STR(a)         #a
#define STRING(a)      STR(a)
#define CONCAT(a,b)    a ## b
#define FUNCTION(name) CONCAT(int_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  /* check that both operands are integers. */
  if (!IS_INT(a) || !IS_INT(b))
    throw(z, ERR_OBJ_BINARY, STRING(F),
          MATTE_TYPE(a)->name,
          MATTE_TYPE(b)->name);

  /* int <op> int => int */
  const long result = int_get_value((Int) a) OP int_get_value((Int) b);
  return (Object) int_new_with_value(z, result);
}

/* undefine the method generator macros.
 */
#undef CONCAT
#undef FUNCTION
#undef OP
#undef F

