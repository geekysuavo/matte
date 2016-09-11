
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

Object matte_disp (Zone z, Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);

  Object x = object_list_get((ObjectList) argin, 0);
  ObjectType type = MATTE_TYPE(x);

  if (type->fn_disp)
    type->fn_disp(z, x);

  return object_list_argout(z, 0);
}

Object matte_sprintf (Zone z, Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);
  Object format = object_list_get((ObjectList) argin, 0);

  String str = NULL;

  if (IS_STRING(format)) {
    str = string_new(z, NULL);
    string_append_objs(str, ((String) format)->data, 1, (ObjectList) argin);
  }
  else
    throw(z, ERR_INVALID_ARGIN);

  return object_list_argout(z, 1, (Object) str);
}

