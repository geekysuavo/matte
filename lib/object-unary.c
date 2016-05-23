
/* define macros for generating an arbitrary unary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Object a) {
  if (!a)
    throw("operand to '%s' is undefined", STRING(F));

  const ObjectType ta = MATTE_TYPE(a);
  obj_unary fn;

  fn = METHOD(ta,F);
  if (fn)
    return fn(a);

  throw("%s(%s) is undefined", STRING(F), ta->name);
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

