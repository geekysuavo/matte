
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_OBJECT_LIST_H__
#define __MATTE_OBJECT_LIST_H__

/* include the object header. */
#include <matte/object.h>

/* IS_OBJECT_LIST: macro to check that an object is a matte object list.
 */
#define IS_OBJECT_LIST(obj) \
  MATTE_TYPE_CHECK(obj, object_list_type())

/* ObjectList: pointer to a struct _ObjectList. */
typedef struct _ObjectList *ObjectList;
struct _ObjectType ObjectList_type;

/* _ObjectList: structure for holding a list of matte objects. used to
 * carry arguments to functions and send results from functions.
 */
struct _ObjectList {
  /* base object. */
  OBJECT_BASE;

  /* @objs: list of generic pointers to matte objects.
   * @n: length of the pointer list.
   */
  Object *objs;
  int n;
};

/* function declarations (object-list.c): */

ObjectType object_list_type (void);

ObjectList object_list_new (Zone z, Object args);

Object object_list_argout (Zone z, int n, ...);

void object_list_delete (Zone z, ObjectList lst);

int object_list_get_length (ObjectList lst);

int object_list_set_length (ObjectList lst, int n);

int object_list_append (ObjectList lst, Object obj);

int object_list_set (ObjectList lst, int i, Object obj);

Object object_list_get (ObjectList lst, int i);

#define object_list_last(lst) \
  object_list_get(lst, object_list_get_length(lst) - 1)

#endif /* !__MATTE_OBJECT_LIST_H__ */

