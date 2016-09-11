
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the struct and exception headers. */
#include <matte/struct.h>
#include <matte/except.h>

/* struct_type(): return a pointer to the struct object type.
 */
ObjectType struct_type (void) {
  /* return the struct address. */
  return &Struct_type;
}

/* struct_new(): allocate a new matte struct.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty struct.
 */
Struct struct_new (Zone z, Object args) {
  /* allocate a new struct. */
  Struct st = (Struct) object_alloc(z, &Struct_type);
  if (!st)
    return NULL;

  /* initialize the struct members. */
  st->objs = NULL;
  st->keys = NULL;
  st->n = 0;

  /* return the new struct. */
  return st;
}

/* struct_delete(): free all memory associated with a matte struct.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @st: matte struct to free.
 */
void struct_delete (Zone z, Struct st) {
  /* return if the struct is null. */
  if (!st)
    return;

  /* free the key strings. */
  for (int i = 0; i < st->n; i++)
    free(st->keys[i]);

  /* free the key and object arrays. */
  free(st->keys);
  free(st->objs);
}

/* struct_get_length(): get the length of a matte struct.
 *
 * arguments:
 *  @st: matte struct to access.
 *
 * returns:
 *  number of elements in the struct, one or more of which may
 *  be null.
 */
int struct_get_length (Struct st) {
  /* return the array size of the struct. */
  return (st ? st->n : 0);
}

/* struct_find(): lookup an object in a struct by its key string.
 *
 * arguments:
 *  @st: matte struct to access.
 *  @key: key string to use for hash lookup.
 *
 * returns:
 *  non-negative object array index within the struct, or -@i-1 if no
 *  object exists in the struct with the given key. in such cases, @i
 *  is the nearest key index that is just greater than the query key
 *  string.
 */
static int struct_find (Struct st, const char *key) {
  /* declare required variables:
   *  @imin: lower and upper binary search bounds.
   *  @i: midpoint object array index for searching.
   *  @cmp: result of comparing with the midpoint key.
   */
  int imin, i, imax, cmp;

  /* return no match if either argument is null. */
  if (!st || st->n == 0 || !key)
    return -1;

  /* initialize the bounds of the binary search. */
  imin = 0;
  imax = st->n - 1;

  /* initialize the binary search midpoint. */
  i = 0;

  /* initialize the binary search comparison result. */
  cmp = 0;

  /* loop until the search bounds have converged. */
  while (imin <= imax) {
    /* locate the midpoint using a floor division. */
    i = (imin + imax) / 2;

    /* compare the midpoint key with the search key. */
    cmp = strcmp(st->keys[i], key);

    /* on equality, return the index.
     * otherwise, refine the bounds.
     */
    if (cmp == 0)
      return i;
    else if (cmp < 0)
      imin = i + 1;
    else
      imax = i - 1;
  }

  /* no match was found. return the nearest key just greater than
   * the search key.
   */
  if (cmp < 0)
    i++;

  /* return the nearest key in adjusted form. */
  return -(i + 1);
}

/* struct_set(): store an object into a matte struct.
 *
 * if the given key does not exist in the struct, insert a new
 * key/object pair.
 *
 * if the key does exist, over-write its object without freeing.
 *
 * arguments:
 *  @st: matte struct to modify.
 *  @key: key string to use for hashing.
 *  @obj: object to store in the struct.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int struct_set (Struct st, const char *key, Object obj) {
  /* declare required variables:
   *  @i: new object array insertion index.
   *  @j: object array general loop counter.
   */
  int i, j;

  /* return failure if any argument is null. */
  if (!st || !key)
    fail(ERR_INVALID_ARGIN);

  /* lookup any existing object by its key string. */
  i = struct_find(st, key);

  /* check if a key was found. */
  if (i >= 0) {
    /* simply store the object in that array location. */
    st->objs[i] = obj;

    /* return success. */
    return 1;
  }

  /* get the key insertion index. */
  i = -(i + 1);

  /* increment the object array size. */
  st->n++;

  /* reallocate the key array. */
  st->keys = (char**) realloc(st->keys, st->n * sizeof(char*));

  /* reallocate the object array. */
  st->objs = (Object*)
    realloc(st->objs, st->n * sizeof(struct _Object*));

  /* check if reallocation failed. */
  if (!st->keys || !st->objs)
    fail(ERR_BAD_ALLOC);

  /* shift the larger key/value pairs down in the array. */
  for (j = st->n - 1; j > i; j--) {
    /* shift the key string and object. */
    st->keys[j] = st->keys[j - 1];
    st->objs[j] = st->objs[j - 1];
  }

  /* store the new key string and object. */
  st->keys[i] = strdup(key);
  st->objs[i] = obj;

  /* return success. */
  return 1;
}

/* struct_get(): get an object from a matte struct, if it exists.
 *
 * arguments:
 *  @st: matte struct to access.
 *  @key: key string to use for hash lookup.
 *
 * returns:
 *  requested element in the struct, which may be null.
 */
Object struct_get (Struct st, const char *key) {
  /* declare required variables:
   *  @i: object array index.
   */
  int i;

  /* lookup the object by its key string. */
  i = struct_find(st, key);

  /* return null if no such key exists. */
  if (i < 0)
    return NULL;

  /* return the requested object in the struct. */
  return st->objs[i];
}

/* struct_get_key(): get an indexed key string from a matte struct.
 *
 * arguments:
 *  @st: matte struct to access.
 *  @i: object/key string index.
 *
 * returns:
 *  constant key string data, which should never be freed.
 */
const char *struct_get_key (Struct st, int i) {
  /* return the requested key in the struct. */
  return (st && i >= 0 && i < st->n ? st->keys[i] : NULL);
}

/* struct_remove(): remove an object from a matte struct. the object
 * will not be freed.
 *
 * arguments:
 *  @st: matte struct to modify.
 *  @key: key string to use for hash lookup.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int struct_remove (Struct st, const char *key) {
  /* declare required variables:
   *  @i, @j: object array indices.
   */
  int i, j;

  /* lookup the object by its key string. */
  i = struct_find(st, key);

  /* return success if the struct is null or empty,
   * or if no such key exists.
   */
  if (i < 0)
    return 1;

  /* free the key string. */
  free(st->keys[i]);

  /* shift the larger key/value pairs up in the array. */
  for (j = i; j < st->n - 1; j++) {
    /* shift the key string and object. */
    st->keys[j] = st->keys[j + 1];
    st->objs[j] = st->objs[j + 1];
  }

  /* decrement the object array size. */
  st->n--;

  /* check if the array size is zero. */
  if (st->n == 0) {
    /* free the arrays. */
    free(st->keys);
    free(st->objs);

    /* set the arrays to null. */
    st->keys = NULL;
    st->objs = NULL;
  }

  /* return success. */
  return 1;
}

/* Struct_type: object type structure for matte structs.
 */
struct _ObjectType Struct_type = {
  "Struct",                                      /* name       */
  sizeof(struct _Struct),                        /* size       */
  0,                                             /* precedence */

  (obj_constructor) struct_new,                  /* fn_new    */
  NULL,                                          /* fn_copy   */
  (obj_destructor)  struct_delete,               /* fn_delete */
  NULL,                                          /* fn_disp   */
  NULL,                                          /* fn_true   */

  NULL,                                          /* fn_plus       */
  NULL,                                          /* fn_minus      */
  NULL,                                          /* fn_uminus     */
  NULL,                                          /* fn_times      */
  NULL,                                          /* fn_mtimes     */
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
  NULL,                                          /* fn_eq         */
  NULL,                                          /* fn_and        */
  NULL,                                          /* fn_or         */
  NULL,                                          /* fn_mand       */
  NULL,                                          /* fn_mor        */
  NULL,                                          /* fn_not        */
  NULL,                                          /* fn_colon      */
  NULL,                                          /* fn_ctranspose */
  NULL,                                          /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

