
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_INT_H__
#define __MATTE_INT_H__

/* include the object header. */
#include <matte/object.h>

/* IS_INT: macro to check that an object is a matte integer.
 */
#define IS_INT(obj) \
  MATTE_TYPE_CHECK(obj, int_type())

/* Int: pointer to a struct _Int. */
typedef struct _Int *Int;
struct _ObjectType Int_type;

/* _Int: structure for holding an integer object.
 */
struct _Int {
  /* base object. */
  OBJECT_BASE;

  /* @value: value of the integer.
   */
  long value;
};

/* function declarations (int.c): */

ObjectType int_type (void);

Int int_new (Zone z, Object args);

Int int_new_with_value (Zone z, long value);

Int int_copy (Zone z, Int i);

long int_get_value (Int i);

void int_set_value (Int i, long value);

#endif /* !__MATTE_INT_H__ */

