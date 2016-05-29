
/* define macros for generating an arbitrary unary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Zone z, Object a) {
  if (!a)
    throw(z, ERR_INVALID_ARGIN);

  const ObjectType ta = MATTE_TYPE(a);
  obj_unary fn;

  fn = METHOD(ta,F);
  if (fn) {
    Object obj = fn(z, a);
    if (!obj)
      return exceptions_get(z);

    return obj;
  }

  throw(z, ERR_OBJ_UNARY, STRING(F), ta->name);
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

