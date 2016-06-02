
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the zone allocator header. */
#include <matte/zone.h>

/* ZONE_UNIT: number of bytes per unit of memory returned by a zone
 * allocator. must be greater than the size of any matte object
 * that the zone is expected to handle.
 */
#define ZONE_UNIT  64

/* FIXME: the zone allocator is really up a creek. in fact, resize()
 * will *invalidate* all prior pointers held by the zone! thus, two
 * things become critically important:
 *
 *  - code should avoid over-allocating objects from their zones.
 *  - zones can never call realloc(), only malloc()... ugh.
 *
 * basically, each zone will hold an array of chunks, where an
 * extra index is stored for each object to indicate its chunk
 * membership.
 *
 * probably also good to ensure that objects are allocated more or
 * less in-order within their zone chunk.
 */

/* resize(): resize the contents of a zone allocator structure.
 *
 * arguments:
 *  @z: pointer to the zone structure to modify.
 *  @n: number of units to reserve for allocation.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int resize (Zone z, unsigned long n) {
  /* never decrease the size of the zone. */
  if (n < z->n)
    return 1;

  /* reallocate the zone data chunk. */
  z->data = realloc(z->data, n * ZONE_UNIT);
  if (!z->data)
    return 0;

  /* reallocate the zone availability array. */
  z->av = (unsigned long*) malloc(n * sizeof(unsigned long));
  if (!z->av)
    return 0;

  /* initialize the contents of the data chunk. */
  memset(((char*) z->data) + z->n, 0, (n - z->n) * ZONE_UNIT);

  /* initialize the contents of the availability array. */
  for (unsigned long i = z->n; i < n; i++)
    z->av[z->nav++] = i;

  /* store the size information into the zone. */
  z->sz = n * ZONE_UNIT;
  z->n = n;

  /* return success. */
  return 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* zone_init(): initialize the contents of a zone allocator structure.
 *
 * arguments:
 *  @z: pointer to the zone structure to initialize.
 *  @n: number of units to reserve for allocation.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int zone_init (Zone z, unsigned long n) {
  /* fail if the zone pointer is null. */
  if (!z) return 0;

  /* initialize the zone data chunk and availability array. */
  z->data = NULL;
  z->av = NULL;

  /* initialize the zone size information. */
  z->n = z->sz = z->nav = 0;
  z->usz = ZONE_UNIT;

  /* resize the zone structure. */
  return resize(z, n);
}

/* zone_alloc(): return a new unit of memory from a zone allocator.
 *
 * if the zone contains no available units, then it's chunk will be
 * resized using realloc() to make room for the new unit.
 *
 * arguments:
 *  @z: pointer to the zone structure to utilize.
 *
 * returns:
 *  pointer into a previously unused location of the zone's data chunk.
 */
void *zone_alloc (Zone z) {
  /* declare required variables:
   *  @i: unit index for the new pointer.
   *  @n: zone size for reallocation.
   *  @ptr: unit pointer.
   */
  unsigned long i, n;
  void *ptr;

  /* use malloc() if the zone pointer is null. */
  if (!z)
    return malloc(ZONE_UNIT);

  /* resize the data chunk, if necessary. */
  if (!z->nav) {
    /* slightly over-reserve to achieve amortized O(1) "appends". */
    n = z->n + 1;
    n += (n >> 3) + (n < 9 ? 3 : 6);

    /* resize the chunk. */
    if (!resize(z, n))
      return NULL;
  }

  /* get the first available unit index. */
  i = z->av[0];
  ptr = ((char*) z->data) + (i * ZONE_UNIT);

  /* swap in a new unit index for the next allocation. */
  z->av[0] = z->av[z->nav - 1];
  z->nav--;

  /* return the new pointer. */
  return ptr;
}

/* zone_free(): release a unit of memory back to a zone allocator.
 *
 * this function performs no bounds checking, so the user must be certain
 * that the memory to be freed was indeed allocated by the passed zone.
 *
 * arguments:
 *  @z: pointer to the zone structure to utilize.
 *  @ptr: pointer to the unit's memory to release.
 */
void zone_free (Zone z, void *ptr) {
  /* use free() if the zone pointer is null. */
  if (!z) {
    free(ptr);
    return;
  }

  /* do not attempt to free null pointers. */
  if (!ptr) return;

  /* compute the offset of the pointer in the zone data chunk. */
  const unsigned long i = ((char*) ptr - (char*) z->data) / ZONE_UNIT;

  /* place the offset back into the availability array. */
  z->av[z->nav++] = i;

  /* re-initialize the released memory. */
  memset(ptr, 0, ZONE_UNIT);
}

/* zone_destroy(): release all allocated memory associated with a zone
 * allocator structure. the members of the zone allocator structure are
 * re-initialized, so it is possible to use zone_init() after calling
 * zone_destroy().
 *
 * arguments:
 *  @z: pointer to the zone structure to destroy.
 */
void zone_destroy (Zone z) {
  /* return if the zone pointer is null. */
  if (!z) return;

  /* reset the data sizes. */
  z->n = z->sz = z->nav = 0;

  /* free the zone data. */
  free(z->data);
  z->data = NULL;

  /* free the availability array. */
  free(z->av);
  z->av = NULL;
}

