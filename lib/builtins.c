
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the matte header. */
#include <matte/matte.h>

/* matte_builtins_init(): initialize built-in symbols with the global
 * compiler symbol table.
 *
 * arguments:
 *  @gs: global symbol table to register symbols with.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int matte_builtins_init (Symbols gs) {
  /* declare required variables:
   *  @ret: return value from all symbol registrations.
   */
  int ret = 1;

  /* register global variables. */
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_INT, "end", -1L);

  /* register global types. */
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_CLASS, "Int");
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_CLASS, "Float");
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_CLASS, "Complex");
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_CLASS, "String");
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_CLASS, "Exception");

  /* register global functions. */
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_FUNC, "disp");
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_FUNC, "sum");
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_FUNC, "sprintf");

  /* return the result. */
  return ret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* string_append_objs(): append matte object data to the end of a matte
 * string in a printf-style manner.
 *
 * arguments:
 *  @s: matte string to modify.
 *  @format: printf-style format string.
 *  @begin: starting index in the object list.
 *  @lst: object list for format string arguments.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_append_objs (String s, char *format, int begin, ObjectList lst) {
  /* declare required variables:
   *  @pa: substring start pointer.
   *  @pb: substring end pointer.
   *  @tmp: temporary character.
   *  @obj: object being printed.
   *  @i: object list index.
   */
  char *pa, *pb, tmp;
  Object obj;
  int i;

  /* validate the input arguments. */
  if (!s || !format)
    fail(ERR_INVALID_ARGIN);

  /* initialize the first substring start pointer. */
  pa = format;
  i = begin;

  /* loop over the format string. */
  do {
    /* find the next format specifier. */
    pb = strchr(pa, '%');

    /* check if a specifier was not found. */
    if (!pb) {
      /* append the last substring. */
      string_append_value(s, pa);
      break;
    }

    /* append the current substring. */
    pb[0] = '\0';
    string_append_value(s, pa);
    pb[0] = '%';

    /* break if the specifier is at the end of the format string. */
    if (pb[1] == '\0') break;

    /* check if the specifier is part of an escaped percent. */
    if (pb[1] == '%') {
      string_append_value(s, "%");
      pa = pb + 2;
      continue;
    }

    /* loop until a type specifier is encountered. */
    pa = pb;
    while (*pb && !(*pb == 'd' || *pb == 'i' || *pb == 'u' || *pb == 'o' ||
                    *pb == 'x' || *pb == 'X' || *pb == 'f' || *pb == 'e' ||
                    *pb == 'E' || *pb == 'g' || *pb == 'G' || *pb == 's')) {
      /* increment the end pointer. */
      pb++;
    }

    /* get an object from the object list. */
    obj = object_list_get(lst, i++);

    /* temporarily null-terminate the format specifier. */
    tmp = pb[1];
    pb[1] = '\0';

    /* write based on the type specifier. */
    if (*pb == 'd' || *pb == 'i') {
      /* signed integer. */
      if (!IS_INT(obj)) return 0;
      string_appendf(s, pa, int_get_value((Int) obj));
    }
    else if (*pb == 'u' || *pb == 'o' || *pb == 'x' || *pb == 'X') {
      /* unsigned integer. */
      if (!IS_INT(obj)) return 0;
      string_appendf(s, pa, (unsigned long) int_get_value((Int) obj));
    }
    else if (*pb == 'f' || *pb == 'e' || *pb == 'E' ||
             *pb == 'g' || *pb == 'G') {
      /* float-formatted values. */
      if (IS_INT(obj)) {
        /* integer. */
        string_appendf(s, pa, (double) int_get_value((Int) obj));
      }
      else if (IS_FLOAT(obj)) {
        /* float. */
        string_appendf(s, pa, float_get_value((Float) obj));
      }
      else if (IS_COMPLEX(obj)) {
        /* complex. */
        const double re = creal(complex_get_value((Complex) obj));
        const double im = cimag(complex_get_value((Complex) obj));
        string_appendf(s, pa, re);
        string_append_value(s, im < 0.0 ? " - " : " + ");
        string_appendf(s, pa, im < 0.0 ? -im : im);
        string_append_value(s, "i");
      }
      else
        return 0;
    }
    else if (*pb == 's') {
      /* string. */
      if (!IS_STRING(obj)) return 0;
      string_append_value(s, string_get_value((String) obj));
    }

    /* un-terminate the format specifier. */
    pb[1] = tmp;

    /* move the substring start pointer past the format string. */
    pa = pb + 1;
  }
  while (1);

  /* return success. */
  return 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Object matte_disp (Zone z, Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);

  Object x = object_list_get((ObjectList) argin, 0);
  ObjectType type = MATTE_TYPE(x);

  if (type->fn_disp)
    type->fn_disp(z, x);

  return object_list_argout(z, 0);
}

Object matte_sum (Zone z, Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);
  Object x = object_list_get((ObjectList) argin, 0);
  Object dim = object_list_get((ObjectList) argin, 1);

  Object y = NULL;

  if (nargin == 1) {
    if (IS_INT(x))
      y = (Object) int_copy(z, (Int) x);
    else if (IS_FLOAT(x))
      y = (Object) float_copy(z, (Float) x);
    else if (IS_COMPLEX(x))
      y = (Object) complex_copy(z, (Complex) x);
    else if (IS_RANGE(x)) {
      Range r = (Range) x;
      long sum = 0;

      for (long elem = r->begin; elem <= r->end; elem += r->step)
        sum += elem;

      y = (Object) int_new_with_value(z, sum);
    }
    else if (IS_VECTOR(x)) {
      Vector v = (Vector) x;
      double sum = 0.0;

      for (long i = 0; i < v->n; i++)
        sum += v->data[i];

      y = (Object) float_new_with_value(z, sum);
    }
  }
  else if (nargin == 2) {
  }
  else
    throw(z, ERR_INVALID_ARGIN);

  return object_list_argout(z, 1, y);
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

