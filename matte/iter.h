
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_ITER_H__
#define __MATTE_ITER_H__

/* include the object header. */
#include <matte/object.h>

/* IS_ITER: macro to check that an object is a matte iterator.
 */
#define IS_ITER(obj) \
  MATTE_TYPE_CHECK(obj, iter_type())

/* Iter: pointer to a struct _Iter. */
typedef struct _Iter *Iter;
struct _ObjectType Iter_type;

/* _Iter: structure for holding an iterator object.
 */
struct _Iter {
  /* base object. */
  OBJECT_BASE;

  /* @obj: master object used to construct iteration values.
   * @val: slave object used to hold iteration values.
   * @i: current iteration index.
   * @n: number of iterations.
   */
  Object obj, val;
  long i, n;
};

/* function declarations (iter.c): */

ObjectType iter_type (void);

Object iter_new (Zone z, Object obj);

int iter_next (Zone z, Iter it);

Object iter_get_value (Iter it);

#endif /* !__MATTE_ITER_H__ */

