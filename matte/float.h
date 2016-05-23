
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_FLOAT_H__
#define __MATTE_FLOAT_H__

/* include the object header. */
#include <matte/object.h>

/* IS_FLOAT: macro to check that an object is a matte float.
 */
#define IS_FLOAT(obj) \
  MATTE_TYPE_CHECK(obj, float_type())

/* Float: pointer to a struct _Float. */
typedef struct _Float *Float;
struct _ObjectType Float_type;

/* _Float: structure for holding a float object.
 */
struct _Float {
  /* base object. */
  OBJECT_BASE;

  /* @value: value of the float.
   */
  double value;
};

/* function declarations (float.c): */

ObjectType float_type (void);

Float float_new (Object args);

Float float_new_with_value (double value);

Float float_copy (Float f);

double float_get_value (Float f);

void float_set_value (Float f, double value);

#endif /* !__MATTE_FLOAT_H__ */

