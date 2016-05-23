
/* define macros for generating an arbitrary ternary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Object a, Object b, Object c) {
  if (!a) throw("first operand to '%s' is undefined", STRING(F));
  if (!b) throw("second operand to '%s' is undefined", STRING(F));
  if (!c) throw("third operand to '%s' is undefined", STRING(F));

  const ObjectType ta = MATTE_TYPE(a);
  const ObjectType tb = MATTE_TYPE(b);
  const ObjectType tc = MATTE_TYPE(c);
  obj_ternary fn;

  if (ta->precedence >= tb->precedence &&
      ta->precedence >= tc->precedence) {
    fn = METHOD(ta,F);
    if (fn)
      return fn(a, b, c);
  }
  else if (tb->precedence >= tc->precedence) {
    fn = METHOD(tb,F);
    if (fn)
      return fn(a, b, c);
  }
  else {
    fn = METHOD(tc,F);
    if (fn)
      return fn(a, b, c);
  }

  throw("%s(%s, %s, %s) is undefined", STRING(F),
        ta->name, tb->name, tc->name);
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

