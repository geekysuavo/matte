
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_ZONE_H__
#define __MATTE_ZONE_H__

/* include the standard library header. */
#include <stdlib.h>

/* Zone: pointer to a struct _Zone. */
typedef struct _Zone *Zone;
typedef struct _Zone ZoneData;

/* _Zone: structure for holding a zone allocation context.
 */
struct _Zone {
  /* @data: chunk of object struct data.
   * @n: number of objects in the zone.
   * @sz: number of bytes in the zone.
   */
  void *data;
  unsigned long n, sz;

  /* @av: array of available zone positions.
   * @nav: size of the availability array.
   */
  unsigned long *av, nav;
};

/* function declarations (zone.c): */

int zone_init (Zone z, unsigned long n);

void *zone_alloc (Zone z);

void zone_free (Zone z, void *ptr);

void zone_destroy (Zone z);

#endif /* !__MATTE_ZONE_H__ */

