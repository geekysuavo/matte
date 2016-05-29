
/* define macros for generating an arbitrary ternary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Zone z, Object a, Object b, Object c) {
  if (!a || !b || !c)
    throw(z, ERR_INVALID_ARGIN);

  const ObjectType ta = MATTE_TYPE(a);
  const ObjectType tb = MATTE_TYPE(b);
  const ObjectType tc = MATTE_TYPE(c);
  obj_ternary fn = NULL;
  Object obj = NULL;

  if (ta->precedence >= tb->precedence &&
      ta->precedence >= tc->precedence)
    fn = METHOD(ta,F);
  else if (tb->precedence >= tc->precedence)
    fn = METHOD(tb,F);
  else
    fn = METHOD(tc,F);

  if (fn) {
    obj = fn(z, a, b, c);
    if (!obj)
      return exceptions_get(z);

    return obj;
  }

  throw(z, ERR_OBJ_TERNARY, STRING(F), ta->name, tb->name, tc->name);
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

