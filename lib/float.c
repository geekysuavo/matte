
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the float and blas headers. */
#include <matte/float.h>
#include <matte/blas.h>

/* include headers for inferior types. */
#include <matte/int.h>
#include <matte/range.h>

/* include headers for superior types. */
#include <matte/vector.h>

/* float_type(): return a pointer to the float object type.
 */
ObjectType float_type (void) {
  /* return the struct address. */
  return &Float_type;
}

/* float_new(): allocate a new matte float.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated float.
 */
Float float_new (Zone z, Object args) {
  /* allocate a new float. */
  Float f = (Float) object_alloc(z, &Float_type);
  if (!f)
    return NULL;

  /* initialize the float value. */
  f->value = 0.0;

  /* return the new float. */
  return f;
}

/* float_new_with_value(): allocate a new matte float with a set value.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @value: initial value of the float.
 *
 * returns:
 *  newly allocated and initialized float.
 */
Float float_new_with_value (Zone z, double value) {
  /* allocate a new float. */
  Float f = float_new(z, NULL);
  if (!f)
    return NULL;

  /* set the float value and return the new float. */
  f->value = value;
  return f;
}

/* float_copy(): allocate a new matte float from another matte float.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @f: matte float to duplicate.
 *
 * returns:
 *  duplicated matte float.
 */
Float float_copy (Zone z, Float f) {
  /* return null if the input argument is null. */
  if (!f)
    return NULL;

  /* allocate a new float with the value of the input object. */
  return float_new_with_value(z, f->value);
}

/* float_get_value(): get the raw value of a matte float.
 *
 * arguments:
 *  @f: matte float to access.
 *
 * returns:
 *  double value of the float object.
 */
inline double float_get_value (Float f) {
  /* return the value of the float. */
  return (f ? f->value : 0.0);
}

/* float_set_value(): set the raw value of a matte float.
 *
 * arguments:
 *  @f: matte float to modify.
 *  @value: new value of the float.
 */
inline void float_set_value (Float f, double value) {
  /* return if the input is null. */
  if (!f)
    return;

  /* set the value of the float. */
  f->value = value;
}

/* float_disp(): display operation for floats.
 */
int float_disp (Zone z, Float f) {
  printf("%lg\n", f->value);
  return 1;
}

/* float_true(): assertion function for floats.
 */
int float_true (Float f) {
  return (f->value ? 1 : 0);
}

/* float_plus(): addition operation for floats.
 */
Object float_plus (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float + float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) +
                                float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* float + int => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) +
                             (double) int_get_value((Int) b));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int + float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) b) +
                             (double) int_get_value((Int) a));
    }
  }

  return NULL;
}

/* float_minus(): subtraction operation for floats.
 */
Object float_minus (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float - float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) -
                                float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* float - int => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) -
                             (double) int_get_value((Int) b));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int - float => float */
      return (Object)
        float_new_with_value(z, (double) int_get_value((Int) a) -
                             float_get_value((Float) b));
    }
  }

  return NULL;
}

/* float_uminus(): unary negation operation for floats.
 */
Float float_uminus (Zone z, Float a) {
  /* compute and return the negation. */
  return float_new_with_value(z, -(a->value));
}

/* float_times(): element-wise multiplication operation for floats.
 */
Object float_times (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float * float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) *
                                float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* float * int => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) *
                             (double) int_get_value((Int) b));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int * float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) b) *
                             (double) int_get_value((Int) a));
    }
  }

  return NULL;
}

/* float_rdivide(): right-division operation for floats.
 */
Object float_rdivide (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float / float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) /
                                float_get_value((Float) b));
    }
    else if (IS_INT(b)) {
      /* float / int => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) a) /
                             (double) int_get_value((Int) b));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int / float => float */
      return (Object)
        float_new_with_value(z, (double) int_get_value((Int) a) /
                             float_get_value((Float) b));
    }
  }

  return NULL;
}

/* float_ldivide(): left-division for floats.
 */
Object float_ldivide (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float \ float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) b) /
                                float_get_value((Float) a));
    }
    else if (IS_INT(b)) {
      /* float \ int => float */
      return (Object)
        float_new_with_value(z, (double) int_get_value((Int) b) /
                             float_get_value((Float) a));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int \ float => float */
      return (Object)
        float_new_with_value(z, float_get_value((Float) b) /
                             (double) int_get_value((Int) a));
    }
  }

  return NULL;
}

/* float_power(): exponentiation operation for floats.
 */
Object float_power (Zone z, Object a, Object b) {
  if (IS_FLOAT(a)) {
    if (IS_FLOAT(b)) {
      /* float ^ float => float */
      return (Object)
        float_new_with_value(z, pow(float_get_value((Float) a),
                                    float_get_value((Float) b)));
    }
    else if (IS_INT(b)) {
      /* float ^ int => float */
      return (Object)
        float_new_with_value(z, pow(float_get_value((Float) a),
                                    (double) int_get_value((Int) b)));
    }
  }
  else if (IS_FLOAT(b)) {
    if (IS_INT(a)) {
      /* int ^ float => float */
      return (Object)
        float_new_with_value(z, pow((double) int_get_value((Int) a),
                                    float_get_value((Float) b)));
    }
  }

  return NULL;
}

/* float_lt(): less-than comparison operation for floats. */
#define F lt
#define OP <
#include "float-cmp.c"

/* float_gt(): greater-than comparison operation for floats. */
#define F gt
#define OP >
#include "float-cmp.c"

/* float_le(): less-than or equal-to comparison operation for floats. */
#define F le
#define OP <=
#include "float-cmp.c"

/* float_ge(): greater-than or equal-to comparison operation for floats. */
#define F ge
#define OP >=
#include "float-cmp.c"

/* float_ne(): inequality comparison operation for floats. */
#define F ne
#define OP !=
#include "float-cmp.c"

/* float_eq(): equality comparison operation for floats. */
#define F eq
#define OP ==
#include "float-cmp.c"

/* float_and(): element-wise logical-and operation for floats. */
#define F and
#define OP &&
#include "float-cmp.c"

/* float_or(): element-wise logical-or operation for floats. */
#define F or
#define OP ||
#include "float-cmp.c"

/* float_mand(): matrix logical-and operation for floats. */
#define F mand
#define OP &&
#include "float-cmp.c"

/* float_mor(): matrix logical-or operation for floats. */
#define F mor
#define OP ||
#include "float-cmp.c"

/* float_not(): logical negation operation for floats.
 */
Int float_not (Zone z, Float a) {
  return int_new_with_value(z, a->value ? 0L : 1L);
}

/* float_colon(): colon operation for floats.
 */
Vector float_colon (Zone z, Object a, Object b, Object c) {
  double begin, step, end;

  if (IS_FLOAT(a))
    begin = float_get_value((Float) a);
  else if (IS_INT(a))
    begin = (double) int_get_value((Int) a);
  else
    return NULL;

  if (IS_FLOAT(b))
    step = float_get_value((Float) b);
  else if (IS_INT(b))
    step = (double) int_get_value((Int) b);
  else
    return NULL;

  if (IS_FLOAT(c))
    end = float_get_value((Float) c);
  else if (IS_INT(c))
    end = (double) int_get_value((Int) c);
  else
    return NULL;

  long n = (long) ceil((end - begin) / step) + 1;
  if (n < 0)
    n = 0;

  Vector x = vector_new_with_length(z, n);
  if (!x)
    return NULL;

  for (long i = 0; i < n; i++, begin += step)
    vector_set(x, i, begin);

  x->tr = CblasTrans;
  return x;
}

/* float_horzcat(): horizontal concatenation function for floats.
 */
Vector float_horzcat (Zone z, int n, va_list vl) {
  Vector x = vector_new(z, NULL);
  if (!x)
    return NULL;

  for (long i = 0, ix = 0; i < n; i++) {
    Object obj = (Object) va_arg(vl, Object);

    if (IS_FLOAT(obj)) {
      if (!vector_set_length(x, vector_get_length(x) + 1)) {
        object_free(z, x);
        return NULL;
      }

      vector_set(x, ix++, float_get_value((Float) obj));
    }
    else if (IS_INT(obj)) {
      if (!vector_set_length(x, vector_get_length(x) + 1)) {
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

/* float_vertcat(): vertical concatenation function for floats.
 */
Vector float_vertcat (Zone z, int n, va_list vl) {
  Vector x = vector_new_with_length(z, n);
  if (!x)
    return NULL;

  for (long i = 0; i < x->n; i++) {
    Object obj = (Object) va_arg(vl, Object);

    if (IS_FLOAT(obj)) {
      vector_set(x, i, float_get_value((Float) obj));
    }
    else if (IS_INT(obj)) {
      vector_set(x, i, (double) int_get_value((Int) obj));
    }
    else {
      object_free(z, x);
      return NULL;
    }
  }

  return x;
}

/* Float_type: object type structure for matte floats.
 */
struct _ObjectType Float_type = {
  "Float",                                       /* name       */
  sizeof(struct _Float),                         /* size       */
  3,                                             /* precedence */

  (obj_constructor) float_new,                   /* fn_new    */
  (obj_constructor) float_copy,                  /* fn_copy   */
  NULL,                                          /* fn_delete */
  (obj_display)     float_disp,                  /* fn_disp   */
  (obj_assert)      float_true,                  /* fn_true   */

  (obj_binary)   float_plus,                     /* fn_plus       */
  (obj_binary)   float_minus,                    /* fn_minus      */
  (obj_unary)    float_uminus,                   /* fn_uminus     */
  (obj_binary)   float_times,                    /* fn_times      */
  (obj_binary)   float_times,                    /* fn_mtimes     */
  (obj_binary)   float_rdivide,                  /* fn_rdivide    */
  (obj_binary)   float_ldivide,                  /* fn_ldivide    */
  (obj_binary)   float_rdivide,                  /* fn_mrdivide   */
  (obj_binary)   float_ldivide,                  /* fn_mldivide   */
  (obj_binary)   float_power,                    /* fn_power      */
  (obj_binary)   float_power,                    /* fn_mpower     */
  (obj_binary)   float_lt,                       /* fn_lt         */
  (obj_binary)   float_gt,                       /* fn_gt         */
  (obj_binary)   float_le,                       /* fn_le         */
  (obj_binary)   float_ge,                       /* fn_ge         */
  (obj_binary)   float_ne,                       /* fn_ne         */
  (obj_binary)   float_eq,                       /* fn_eq         */
  (obj_binary)   float_and,                      /* fn_and        */
  (obj_binary)   float_or,                       /* fn_or         */
  (obj_binary)   float_mand,                     /* fn_mand       */
  (obj_binary)   float_mor,                      /* fn_mor        */
  (obj_unary)    float_not,                      /* fn_not        */
  (obj_ternary)  float_colon,                    /* fn_colon      */
  (obj_unary)    float_copy,                     /* fn_ctranspose */
  (obj_unary)    float_copy,                     /* fn_transpose  */
  (obj_variadic) float_horzcat,                  /* fn_horzcat    */
  (obj_variadic) float_vertcat,                  /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

