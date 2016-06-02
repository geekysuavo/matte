
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_ZONE_H__
#define __MATTE_ZONE_H__

/* include the standard library headers. */
#include <stdlib.h>
#include <string.h>

/* Zone: pointer to a struct _Zone. */
typedef struct _Zone *Zone;
typedef struct _Zone ZoneData;

/* _Zone: structure for holding a zone allocation context.
 */
struct _Zone {
  /* @data: block of object struct data.
   * @dend: end of the object data block.
   * @n: number of objects in the zone.
   */
  void *data, *dend;
  unsigned long n;

  /* @av: array of available zone positions.
   * @nav: size of the availability array.
   */
  unsigned long *av, nav;

  /* @next: next block of the zone allocation context.
   */
  Zone next;
};

/* function declarations (zone.c): */

int zone_init (Zone z, unsigned long n);

void *zone_alloc (Zone z);

void zone_free (Zone z, void *ptr);

void zone_destroy (Zone z);

#endif /* !__MATTE_ZONE_H__ */

