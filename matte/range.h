
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_RANGE_H__
#define __MATTE_RANGE_H__

/* include the object header. */
#include <matte/object.h>

/* IS_RANGE: macro to check that an object is a matte range.
 */
#define IS_RANGE(obj) \
  MATTE_TYPE_CHECK(obj, range_type())

/* Int: pointer to a struct _Range. */
typedef struct _Range *Range;
struct _ObjectType Range_type;

/* _Range: structure for holding ranges of integers.
 */
struct _Range {
  /* base object. */
  OBJECT_BASE;

  /* @begin: first value of the range.
   * @step: step value of the range.
   * @end: last value of the range.
   */
  long begin, step, end;
};

/* function declarations (Range.c): */

ObjectType range_type (void);

Range range_new (Object args);

Range range_copy (Range r);

long range_get_begin (Range r);

long range_get_step (Range r);

long range_get_end (Range r);

void range_get (Range r, long *begin, long *step, long *end);

long range_get_length (Range r);

void range_set_begin (Range r, long value);

void range_set_step (Range r, long value);

void range_set_end (Range r, long value);

void range_set (Range r, long begin, long step, long end);

long range_any (Range r);

long range_all (Range r);

#endif /* !__MATTE_RANGE_H__ */

