
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the matte header. */
#include <matte/matte.h>

/* matte_globals_init(): initialize built-in global variables with the
 * global compiler symbol table.
 *
 * arguments:
 *  @gs: global symbol table to register symbols with.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int matte_globals_init (Symbols gs) {
  /* declare required variables:
   *  @ret: return value from all symbol registrations.
   */
  int ret = 1;

  /* register the global variables. */
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_INT, "end", -1L);

  /* return the result. */
  return ret;
}

/* matte_builtins_init(): initialize built-in functions with the global
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

  /* register global functions. */
  ret = ret && symbols_add(gs, SYMBOL_GLOBAL_FUNC, "sum");

  /* return the result. */
  return ret;
}

Object matte_sum (Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);
  Object x = object_list_get((ObjectList) argin, 0);
  Object dim = object_list_get((ObjectList) argin, 1);

  Object argout = (Object) object_list_new(NULL);
  Object y = NULL;

  if (nargin == 1) {
    if (IS_INT(x))
      y = (Object) int_copy((Int) x);
    else if (IS_FLOAT(x))
      y = (Object) float_copy((Float) x);
    else if (IS_COMPLEX(x))
      y = (Object) complex_copy((Complex) x);
    else if (IS_RANGE(x)) {
      Range r = (Range) x;
      long sum = 0;

      for (long elem = r->begin; elem <= r->end; elem += r->step)
        sum += elem;

      y = (Object) int_new_with_value(sum);
    }
    else if (IS_VECTOR(x)) {
      Vector v = (Vector) x;
      double sum = 0.0;

      for (long i = 0; i < v->n; i++)
        sum += v->data[i];

      y = (Object) float_new_with_value(sum);
    }
  }
  else if (nargin == 2) {
  }

  object_list_set((ObjectList) argout, 0, y);
  return argout;
}

