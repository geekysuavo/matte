
/* define macros for generating an integer comparison method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(int_, name)

Int FUNCTION(F) (Zone z, Int a, Int b) {
  /* int <op> int => int */
  return int_new_with_value(z, a->value OP b->value);
}

/* undefine the method generator macros.
 */
#undef CONCAT
#undef FUNCTION
#undef OP
#undef F

