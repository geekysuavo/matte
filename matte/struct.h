
/* ensure once-only inclusion. */
#ifndef __MATTE_STRUCT_H__
#define __MATTE_STRUCT_H__

/* include the object header. */
#include <matte/object.h>

/* IS_STRUCT: macro to check that an object is a matte struct.
 */
#define IS_STRUCT(obj) \
  MATTE_TYPE_CHECK(obj, struct_type())

/* Struct: pointer to a struct _Struct. */
typedef struct _Struct *Struct;
struct _ObjectType Struct_type;

/* _Struct: structure for holding a string-hashed list of matte objects.
 */
struct _Struct {
  /* base object. */
  OBJECT_BASE;

  /* @objs: list of generic pointers to matte objects.
   * @keys: hashing strings for each stored object.
   * @n: length of the pointer list.
   */
  Object *objs;
  char **keys;
  int n;
};

/* function declarations (struct.c): */

ObjectType struct_type (void);

Struct struct_new (Zone z, Object args);

void struct_delete (Zone z, Struct st);

int struct_get_length (Struct st);

int struct_insert (Struct st, const char *key, Object obj);

int struct_set (Struct st, const char *key, Object obj);

Object struct_get (Struct st, const char *key);

const char *struct_get_key (Struct st, int i);

int struct_remove (Struct st, const char *key);

#endif /* !__MATTE_STRUCT_H__ */

