
/* define macros for generating an arbitrary binary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  if (!a || !b)
    return NULL;

  const ObjectType ta = MATTE_TYPE(a);
  const ObjectType tb = MATTE_TYPE(b);
  obj_binary fn;

  if (ta->precedence >= tb->precedence) {
    fn = METHOD(ta,F);
    if (fn)
      return fn(z, a, b);
  }
  else {
    fn = METHOD(tb,F);
    if (fn)
      return fn(z, a, b);
  }

  return NULL;
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

