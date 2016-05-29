
/* define macros for generating an arbitrary binary method handler.
 */
#define STR(a)            #a
#define STRING(a)         STR(a)
#define CONCAT(a,b)       a ## b
#define FUNCTION(name)    CONCAT(object_, name)
#define METHOD(type,name) CONCAT(type->fn_, name)

Object FUNCTION(F) (Zone z, int n, ...) {
  ObjectType t, tmax;
  obj_variadic fn;
  Object obj;
  va_list vl;
  int i;

  if (n <= 0)
    return NULL;

  va_start(vl, n);
  tmax = MATTE_TYPE(va_arg(vl, Object));
  for (i = 1; i < n; i++) {
    t = MATTE_TYPE(va_arg(vl, Object));
    if (t->precedence > tmax->precedence)
      tmax = t;
  }

  va_end(vl);

  fn = METHOD(tmax,F);
  if (fn) {
    va_start(vl, n);
    obj = fn(z, n, vl);
    va_end(vl);

    if (!obj)
      return exceptions_get(z);

    return obj;
  }

  throw(z, ERR_OBJ_VARIADIC, STRING(F), tmax->name);
}

/* undefine the method handler generation macros.
 */
#undef STR
#undef STRING
#undef CONCAT
#undef METHOD
#undef FUNCTION
#undef F

