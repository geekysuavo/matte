
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the range and exception headers. */
#include <matte/range.h>
#include <matte/except.h>

/* include headers for inferior types. */
#include <matte/int.h>

/* include headers for superior types. */
#include <matte/vector.h>

/* range_type(): return a pointer to the range object type.
 */
ObjectType range_type (void) {
  /* return the struct address. */
  return &Range_type;
}

/* range_new(): allocate a new matte range.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated range.
 */
Range range_new (Zone z, Object args) {
  /* allocate a new range. */
  Range r = (Range) object_alloc(z, &Range_type);
  if (!r)
    return NULL;

  /* initialize the range values. */
  r->begin = r->step = r->end = 0L;

  /* return the new range. */
  return r;
}

/* range_copy(): allocate a new matte range from another matte range.
 *
 * arguments:
 *  @r: matte range to duplicate.
 *
 * returns:
 *  duplicated matte range.
 */
Range range_copy (Zone z, Range r) {
  /* return null if the input argument is null. */
  if (!r)
    return NULL;

  /* allocate a new range. */
  Range rnew = range_new(z, NULL);
  if (!rnew)
    return NULL;

  /* set the values of the range. */
  rnew->begin = r->begin;
  rnew->step  = r->step;
  rnew->end   = r->end;

  /* return the new range. */
  return rnew;
}

/* range_get_begin(): get the first value of a matte range.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  long integer value of the range start.
 */
inline long range_get_begin (Range r) {
  /* return the value from the range. */
  return (r ? r->begin : 0L);
}

/* range_get_step(): get the step value of a matte range.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  long integer value of the range step.
 */
inline long range_get_step (Range r) {
  /* return the value from the range. */
  return (r ? r->step : 0L);
}

/* range_get_end(): get the last value of a matte range.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  long integer value of the range stop.
 */
inline long range_get_end (Range r) {
  /* return the value from the range. */
  return (r ? r->end : 0L);
}

/* range_get(): get all three values of a matte range.
 *
 * arguments:
 *  @r: matte range to access.
 *  @begin: pointer to store the first value of the range.
 *  @step: pointer to store the step value of the range.
 *  @end: pointer to store the end value of the range.
 */
inline void range_get (Range r, long *begin, long *step, long *end) {
  /* return if the input is null. */
  if (!r)
    return;

  /* extract the values from the range. */
  *begin = r->begin;
  *step  = r->step;
  *end   = r->end;
}

/* range_get_length(): get the number of elements spanned by a range.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  number of elements in the range.
 */
inline long range_get_length (Range r) {
  /* return if the input is null. */
  if (!r)
    return 0;

  /* compute the (non-negative) number of elements in the range. */
  long n = (r->end - r->begin) / r->step + 1;
  if (n < 0)
    n = 0;

  /* return the number of elements. */
  return n;
}

/* range_set_begin(): set the first value of a matte range.
 *
 * arguments:
 *  @r: matte range to modify.
 *  @value: new value to store.
 */
inline void range_set_begin (Range r, long value) {
  /* return if the input is null. */
  if (!r)
    return;

  /* store the value in the range. */
  r->begin = value;
}

/* range_set_step(): set the step value of a matte range.
 *
 * arguments:
 *  @r: matte range to modify.
 *  @value: new value to store.
 */
inline void range_set_step (Range r, long value) {
  /* return if the input is null. */
  if (!r)
    return;

  /* store the value in the range. */
  r->step = value;
}

/* range_set_end(): set the last value of a matte range.
 *
 * arguments:
 *  @r: matte range to modify.
 *  @value: new value to store.
 */
inline void range_set_end (Range r, long value) {
  /* return if the input is null. */
  if (!r)
    return;

  /* store the value in the range. */
  r->end = value;
}

/* range_set(): set all three values of a matte range.
 *
 * arguments:
 *  @r: matte range to modify.
 *  @begin: first value of the range.
 *  @step: step value of the range.
 *  @end: end value of the range.
 */
inline void range_set (Range r, long begin, long step, long end) {
  /* return if the input is null. */
  if (!r)
    return;

  /* store the values in the range. */
  r->begin = begin;
  r->step  = step;
  r->end   = end;
}

/* range_any(): inline short-circuit evaluator that any elements of
 * a range are nonzero.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  long integer indicating the result.
 */
inline long range_any (Range r) {
  /* loop over the elements of the range to evaluate the result. */
  for (long elem = r->begin; elem < r->end; elem += r->step) {
    /* short-circuit to true if any element is nonzero. */
    if (elem) return 1L;
  }

  /* return false. */
  return 0L;
}

/* range_all(): inline short-circuit evaluator that all elements of
 * a range are nonzero.
 *
 * arguments:
 *  @r: matte range to access.
 *
 * returns:
 *  long integer indicating the result.
 */
inline long range_all (Range r) {
  /* loop over the elements of the range to evaluate the test. */
  for (long elem = r->begin; elem < r->end; elem += r->step) {
    /* short-circuit to false if any element is zero. */
    if (!elem) return 0L;
  }

  /* return true. */
  return 1L;
}

/* range_disp(): display function for ranges.
 */
int range_disp (Zone z, Range r) {
  /* print the range contents and return success. */
  printf("%ld : %ld : %ld\n", r->begin, r->step, r->end);
  return 1;
}

/* range_true(): assertion function for ranges.
 */
int range_true (Range r) {
  return (int) range_all(r);
}

/* range_plus(): addition function for ranges.
 */
Object range_plus (Zone z, Object a, Object b) {
  if (IS_RANGE(a) && IS_RANGE(b)) {
    /* range + range => range */
    if (range_get_length((Range) a) != range_get_length((Range) b))
      throw(z, ERR_SIZE_MISMATCH);

    const long lb = range_get_begin((Range) a) + range_get_begin((Range) b);
    const long ls = range_get_step((Range) a)  + range_get_step((Range) b);
    const long le = range_get_end((Range) a)   + range_get_end((Range) b);

    Range r = range_new(z, NULL);
    range_set(r, lb, ls, le);
    return (Object) r;
  }
  else if (IS_RANGE(a) && IS_INT(b)) {
    /* range + int => range */
    const long bval = int_get_value((Int) b);

    Range r = range_new(z, NULL);
    range_set(r, range_get_begin((Range) a) + bval,
                 range_get_step((Range) a),
                 range_get_end((Range) a) + bval);

    return (Object) r;
  }
  else if (IS_RANGE(b) && IS_INT(a)) {
    /* int + range => range */
    const long aval = int_get_value((Int) a);

    Range r = range_new(z, NULL);
    range_set(r, range_get_begin((Range) b) + aval,
                 range_get_step((Range) b),
                 range_get_end((Range) b) + aval);

    return (Object) r;
  }

  return NULL;
}

/* range_minus(): subtraction function for ranges.
 */
Object range_minus (Zone z, Object a, Object b) {
  if (IS_RANGE(a) && IS_RANGE(b)) {
    /* range - range => range */
    if (range_get_length((Range) a) != range_get_length((Range) b))
      throw(z, ERR_SIZE_MISMATCH);

    const long lb = range_get_begin((Range) a) - range_get_begin((Range) b);
    const long ls = range_get_step((Range) a)  - range_get_step((Range) b);
    const long le = range_get_end((Range) a)   - range_get_end((Range) b);

    Range r = range_new(z, NULL);
    range_set(r, lb, ls, le);
    return (Object) r;
  }
  else if (IS_RANGE(a) && IS_INT(b)) {
    /* range - int => range */
    const long bval = int_get_value((Int) b);

    Range r = range_new(z, NULL);
    range_set(r, range_get_begin((Range) a) - bval,
                 range_get_step((Range) a),
                 range_get_end((Range) a) - bval);

    return (Object) r;
  }
  else if (IS_RANGE(b) && IS_INT(a)) {
    /* int - range => range */
    const long aval = int_get_value((Int) a);

    Range r = range_new(z, NULL);
    range_set(r, aval - range_get_begin((Range) b),
                       -range_get_step((Range) b),
                 aval - range_get_end((Range) b));

    return (Object) r;
  }

  return NULL;
}

/* range_uminus(): unary negation function for ranges.
 */
Range range_uminus (Zone z, Range a) {
  Range r = range_new(z, NULL);
  range_set(r, -range_get_begin(a),
               -range_get_step(a),
               -range_get_end(a));

  return r;
}

/* range_times(): multiplication function for ranges.
 */
Range range_times (Zone z, Object a, Object b) {
  if (IS_RANGE(a) && IS_INT(b)) {
    /* range .* int => range */
    const long bval = int_get_value((Int) b);
    Range r = range_new(z, NULL);

    range_set(r, range_get_begin((Range) a) * bval,
                 range_get_step((Range) a)  * bval,
                 range_get_end((Range) a)   * bval);

    return r;
  }
  else if (IS_RANGE(b) && IS_INT(a)) {
    /* int .* range => range */
    const long aval = int_get_value((Int) a);
    Range r = range_new(z, NULL);

    range_set(r, range_get_begin((Range) b) * aval,
                 range_get_step((Range) b)  * aval,
                 range_get_end((Range) b)   * aval);

    return r;
  }

  return NULL;
}

/* range_eq(): equality operation for ranges.
 */
Int range_eq (Zone z, Object a, Object b) {
  if (!IS_RANGE(a) || !IS_RANGE(b))
    return int_new_with_value(z, 0L);

  if ((range_get_begin((Range) a) != range_get_begin((Range) b)) ||
      (range_get_step((Range) a)  != range_get_step((Range) b)) ||
      (range_get_end((Range) a)   != range_get_end((Range) b)))
    return int_new_with_value(z, 0L);

  return int_new_with_value(z, 1L);
}

/* range_mand(): matrix logical-and operation for ranges.
 */
Int range_mand (Zone z, Object a, Object b) {
  if (IS_RANGE(a)) {
    if (IS_INT(b)) {
      /* range && int => int */
      if (range_all((Range) a) && int_get_value((Int) b))
        return int_new_with_value(z, 1L);

      return int_new_with_value(z, 0L);
    }
    else if (IS_RANGE(b)) {
      /* range && range => int */
      if (range_all((Range) a) && range_all((Range) b))
        return int_new_with_value(z, 1L);

      return int_new_with_value(z, 0L);
    }
  }
  else if (IS_RANGE(b)) {
    if (IS_INT(a)) {
      /* int && range => int */
      if (int_get_value((Int) a) && range_all((Range) b))
        return int_new_with_value(z, 1L);

      return int_new_with_value(z, 0L);
    }
  }

  return NULL;
}

/* range_mor(): matrix logical-or operation for ranges.
 */
Int range_mor (Zone z, Object a, Object b) {
  if (IS_RANGE(a)) {
    if (IS_INT(b)) {
      /* range || int => int */
      if (range_all((Range) a) || int_get_value((Int) b))
        return int_new_with_value(z, 1L);

      return int_new_with_value(z, 0L);
    }
    else if (IS_RANGE(b)) {
      /* range || range => int */
      if (range_all((Range) a) || range_all((Range) b))
        return int_new_with_value(z, 1L);

      return int_new_with_value(z, 0L);
    }
  }
  else if (IS_RANGE(b)) {
    if (IS_INT(a)) {
      /* int || range => int */
      if (int_get_value((Int) a) || range_all((Range) b))
        return int_new_with_value(z, 1L);

      return int_new_with_value(z, 0L);
    }
  }

  return NULL;
}

/* range_horzcat(): horizontal concatenation function for ranges.
 */
Vector range_horzcat (Zone z, int n, va_list vl) {
  Vector x = vector_new(z, NULL);
  if (!x)
    return NULL;

  for (long i = 0, ix = 0; i < n; i++) {
    Object obj = (Object) va_arg(vl, Object);

    if (IS_INT(obj)) {
      if (!vector_set_length(x, x->n + 1)) {
        object_free(z, x);
        return NULL;
      }

      vector_set(x, ix++, (double) int_get_value((Int) obj));
    }
    else if (IS_RANGE(obj)) {
      Range r = (Range) obj;
      long nr = range_get_length(r);

      if (!vector_set_length(x, x->n + nr)) {
        object_free(z, x);
        return NULL;
      }

      for (long elem = r->begin; elem <= r->end; elem += r->step)
        vector_set(x, ix++, (double) elem);
    }
    else {
      object_free(z, x);
      return NULL;
    }
  }

  x->tr = CblasTrans;
  return x;
}

/* range_vertcat(): vertical concatenation function for ranges.
 */
Vector range_vertcat (Zone z, int n, va_list vl) {
  if (n != 1) {
    error(ERR_INVALID_ARGIN)
    return NULL;
  }

  Vector x = vector_new_from_range(z, (Range) va_arg(vl, Range));
  if (!x)
    return NULL;

  x->tr = CblasTrans;
  return x;
}

/* Range_type: object type structure for matte ranges.
 */
struct _ObjectType Range_type = {
  "Range",                                       /* name       */
  sizeof(struct _Range),                         /* size       */
  2,                                             /* precedence */

  (obj_constructor) range_new,                   /* fn_new    */
  (obj_constructor) range_copy,                  /* fn_copy   */
  NULL,                                          /* fn_delete */
  (obj_display)     range_disp,                  /* fn_disp   */
  (obj_assert)      range_true,                  /* fn_true   */

  (obj_binary)   range_plus,                     /* fn_plus       */
  (obj_binary)   range_minus,                    /* fn_minus      */
  (obj_unary)    range_uminus,                   /* fn_uminus     */
  (obj_binary)   range_times,                    /* fn_times      */
  (obj_binary)   range_times,                    /* fn_mtimes     */
  NULL,                                          /* fn_rdivide    */
  NULL,                                          /* fn_ldivide    */
  NULL,                                          /* fn_mrdivide   */
  NULL,                                          /* fn_mldivide   */
  NULL,                                          /* fn_power      */
  NULL,                                          /* fn_mpower     */
  NULL,                                          /* fn_lt         */
  NULL,                                          /* fn_gt         */
  NULL,                                          /* fn_le         */
  NULL,                                          /* fn_ge         */
  NULL,                                          /* fn_ne         */
  (obj_binary)   range_eq,                       /* fn_eq         */
  NULL,                                          /* fn_and        */
  NULL,                                          /* fn_or         */
  (obj_binary)   range_mand,                     /* fn_mand       */
  (obj_binary)   range_mor,                      /* fn_mor        */
  NULL,                                          /* fn_not        */
  NULL,                                          /* fn_colon      */
  NULL,                                          /* fn_ctranspose */
  NULL,                                          /* fn_transpose  */
  (obj_variadic) range_horzcat,                  /* fn_horzcat    */
  (obj_variadic) range_vertcat,                  /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

