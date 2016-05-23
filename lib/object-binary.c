
/* define macros for generating an arbitrary binary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Object a, Object b) {
  if (!a) throw("first operand to '%s' is undefined", STRING(F));
  if (!b) throw("second operand to '%s' is undefined", STRING(F));

  const ObjectType ta = MATTE_TYPE(a);
  const ObjectType tb = MATTE_TYPE(b);
  obj_binary fn;

  if (ta->precedence >= tb->precedence) {
    fn = METHOD(ta,F);
    if (fn)
      return fn(a, b);
  }
  else {
    fn = METHOD(tb,F);
    if (fn)
      return fn(a, b);
  }

  throw("%s(%s, %s) is undefined", STRING(F), ta->name, tb->name);
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

