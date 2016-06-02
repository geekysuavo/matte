
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

  /* initialize the zone data block and availability array. */
  z->data = NULL;
  z->av = NULL;

  /* initialize the zone size information. */
  z->n = z->nav = 0;

  /* initialize the zone chain pointer. */
  z->next = NULL;

  /* allocate the zone data block. */
  z->data = malloc(n * ZONE_UNIT);
  if (!z->data)
    return 0;

  /* allocate the zone availability array. */
  z->av = (unsigned long*) malloc(n * sizeof(unsigned long));
  if (!z->av)
    return 0;

  /* initialize the contents of the data block. */
  memset(z->data, 0, n * ZONE_UNIT);

  /* initialize the contents of the availability array. */
  for (unsigned long i = 0; i < n; i++)
    z->av[i] = i;

  /* store the size information into the zone. */
  z->dend = ((char*) z->data) + (n * ZONE_UNIT);
  z->n = z->nav = n;

  /* return success. */
  return 1;
}

/* zone_alloc(): return a new unit of memory from a zone allocator.
 *
 * if the zone contains no available units, then a new block will be
 * added to the zone, and the new unit will be returned from the new
 * block.
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
   *  @n: zone unit count for expansion.
   *  @sz: memory block size.
   *  @ptr: unit pointer.
   */
  unsigned long i, n, sz;
  Zone zsrc;
  void *ptr;

  /* use malloc() if the zone pointer is null. */
  if (!z)
    return malloc(ZONE_UNIT);

  /* locate the first non-empty zone block. */
  zsrc = z;
  while (!zsrc->nav && zsrc->next)
    zsrc = zsrc->next;

  /* check if no blocks are non empty. */
  if (!zsrc->nav) {
    /* reserve a slightly larger block than the previous one. */
    n = zsrc->nav;
    n += (n >> 3) + (n < 9 ? 3 : 6);

    /* allocate a pointer to the next block. */
    sz = sizeof(ZoneData) + (n * (ZONE_UNIT + sizeof(unsigned long)));
    zsrc->next = (Zone) malloc(sz);
    if (!zsrc->next)
      return NULL;

    /* move to the newly allocated block. */
    zsrc = zsrc->next;
    zsrc->next = NULL;

    /* set the zone data pointers into the allocated block. */
    zsrc->data = ((char*) zsrc) + sizeof(ZoneData);
    zsrc->dend = ((char*) zsrc->data) + (n * ZONE_UNIT);
    zsrc->n = n;

    /* set the availability array pointer into the allocated block. */
    zsrc->av = (unsigned long*) zsrc->dend;
    zsrc->nav = n;

    /* initialize the contents of the data block. */
    memset(zsrc->data, 0, n * ZONE_UNIT);

    /* initialize the contents of the availability array. */
    for (unsigned long i = 0; i < n; i++)
      zsrc->av[i] = i;
  }

  /* get the first available unit index. */
  i = zsrc->av[0];
  ptr = ((char*) zsrc->data) + (i * ZONE_UNIT);

  /* swap in a new unit index for the next allocation. */
  zsrc->av[0] = zsrc->av[zsrc->nav - 1];
  zsrc->nav--;

  /* return the new pointer. */
  return ptr;
}

/* zone_free(): release a unit of memory back to a zone allocator.
 *
 * arguments:
 *  @z: pointer to the zone structure to utilize.
 *  @ptr: pointer to the unit's memory to release.
 */
void zone_free (Zone z, void *ptr) {
  /* do not attempt to free null pointers. */
  if (!ptr) return;

  /* use free() if the zone pointer is null. */
  if (!z) {
    free(ptr);
    return;
  }

  /* loop until the correct block is identified. */
  Zone zsrc = z;
  while (zsrc) {
    /* check if the pointer is within the current block. */
    if (ptr >= zsrc->data && ptr < zsrc->dend) {
      /* compute the offset of the pointer in the zone data block. */
      const unsigned long i = ((char*) ptr - (char*) zsrc->data) / ZONE_UNIT;

      /* place the offset back into the availability array. */
      zsrc->av[zsrc->nav++] = i;

      /* re-initialize the released memory. */
      memset(ptr, 0, ZONE_UNIT);

      /* the pointer is freed. return without checking the next blocks. */
      return;
    }

    /* no dice. move to the next block. */
    zsrc = zsrc->next;
  }
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
  z->n = z->nav = 0;

  /* free the zone data. */
  free(z->data);
  z->data = NULL;
  z->dend = NULL;

  /* free the availability array. */
  free(z->av);
  z->av = NULL;

  /* free the next block. */
  if (z->next) {
    zone_destroy(z->next);
    free(z->next);
    z->next = NULL;
  }
}

