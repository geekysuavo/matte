
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the object list and exception headers. */
#include <matte/object-list.h>
#include <matte/except.h>

/* object_list_type(): return a pointer to the object list object type.
 */
ObjectType object_list_type (void) {
  /* return the struct address. */
  return &ObjectList_type;
}

/* object_list_new(): allocate a new matte object list.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty object list.
 */
ObjectList object_list_new (Zone z, Object args) {
  /* allocate a new object list. */
  ObjectList lst = (ObjectList) object_alloc(z, &ObjectList_type);
  if (!lst)
    return NULL;

  /* initialize the object list members. */
  lst->objs = NULL;
  lst->n = 0;

  /* return the new object list. */
  return lst;
}

/* object_list_new_with_args(): allocate and fill a new matte object list
 * with a set number of objects.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @cp: whether or not to copy the objects into the list.
 *  @n: number of objects to place in the list.
 *  @...: list of objects to place in the list.
 *
 * returns:
 *  newly allocated object list, cast to a base object.
 */
Object object_list_new_with_args (Zone z, bool cp, int n, ...) {
  /* declare required variables:
   *  @lst: new object list.
   *  @vl: variable-length argument list.
   *  @i: argument loop counter.
   */
  ObjectType type;
  ObjectList lst;
  Object obj;
  va_list vl;
  int i;
  
  /* allocate a new object list. */
  lst = object_list_new(z, NULL);
  if (!lst)
    return exceptions_get(z);

  /* set the length of the list. */
  if (!object_list_set_length(lst, n)) {
    object_free(z, lst);
    return exceptions_get(z);
  }

  /* loop over the arguments. */
  va_start(vl, n);
  for (i = 0; i < n; i++) {
    /* get the object and its type. */
    obj = (Object) va_arg(vl, Object);
    type = MATTE_TYPE(obj);

    /* copy or store the object into the list. */
    if (cp && type->fn_copy)
      lst->objs[i] = type->fn_copy(z, obj);
    else
      lst->objs[i] = obj;
  }

  /* return the new object list as a base object. */
  va_end(vl);
  return (Object) lst;
}

/* object_list_delete(): free all memory associated with a matte object list.
 * elements contained by the list are not freed.
 *
 * arguments:
 *  @lst: matte object list to free.
 */
void object_list_delete (Zone z, ObjectList lst) {
  /* return if the object list is null. */
  if (!lst)
    return;

  /* free the object array. */
  free(lst->objs);
}

/* object_list_get_length(): get the length of a matte object list.
 *
 * arguments:
 *  @lst: matte object list to access.
 *
 * returns:
 *  number of elements in the object list, one or more of which may
 *  be null.
 */
int object_list_get_length (ObjectList lst) {
  /* return the array size of the object list. */
  return (lst ? lst->n : 0);
}

/* object_list_set_length(): set the length of a matte object list.
 *
 * arguments:
 *  @lst: matte object list to modify.
 *  @n: new length of the object list.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int object_list_set_length (ObjectList lst, int n) {
  /* declare required variables:
   *  @i: object array index.
   */
  int i;

  /* validate the input arguments. */
  if (!lst || n < 0)
    fail(ERR_INVALID_ARGIN);

  /* reallocate the object array. */
  lst->objs = (Object*)
    realloc(lst->objs, n * sizeof(struct _Object*));

  /* check if reallocation failed. */
  if (!lst->objs) {
    /* set the new array length. */
    lst->n = n;

    /* return failure. */
    fail(ERR_BAD_ALLOC);
  }

  /* check if the object list expanded. */
  if (n > lst->n) {
    /* initialize the new elements of the object list. */
    for (i = lst->n; i < n; i++)
      lst->objs[i] = NULL;
  }

  /* set the new length. */
  lst->n = n;

  /* return success. */
  return 1;
}

/* object_list_append(): append an object to the end of a matte object list.
 *
 * arguments:
 *  @lst: matte object list to modify.
 *  @obj: matte object to store.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int object_list_append (ObjectList lst, Object obj) {
  /* validate the input arguments. */
  if (!lst || !obj)
    fail(ERR_INVALID_ARGIN);

  /* increment the object array size. */
  lst->n++;

  /* reallocate the object array. */
  lst->objs = (Object*)
    realloc(lst->objs, lst->n * sizeof(struct _Object*));

  /* check if reallocation failed. */
  if (!lst->objs)
    fail(ERR_BAD_ALLOC);

  /* store the object in the list. */
  lst->objs[lst->n - 1] = obj;

  /* return success. */
  return 1;
}

/* object_list_set(): store an object in a matte object list.
 *
 * arguments:
 *  @lst: matte object list to modify.
 *  @i: index of the stored object.
 *  @obj: matte object to store
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int object_list_set (ObjectList lst, int i, Object obj) {
  /* validate the input arguments. */
  if (!lst || i < 0)
    fail(ERR_INVALID_ARGIN);

  /* check if the index is out of bounds. */
  if (i >= lst->n) {
    /* resize the object array. */
    if (!object_list_set_length(lst, i + 1))
      return 0;
  }

  /* store the object in the list. */
  lst->objs[i] = obj;

  /* return success. */
  return 1;
}

/* object_list_get(): get an object from a matte object list.
 *
 * arguments:
 *  @lst: matte object list to access.
 *  @i: index of the requested object.
 *
 * returns:
 *  requested element in the object list, which may be null.
 */
Object object_list_get (ObjectList lst, int i) {
  /* return the requested object in the list. */
  return (lst && i >= 0 && i < lst->n ? lst->objs[i] : NULL);
}

/* object_list_disp(): display function for object lists.
 */
int object_list_disp (Zone z, ObjectList lst, const char *var) {
  /* start the list. */
  printf("%s = {", var);

  /* print the list objects. */
  for (int i = 0; i < lst->n; i++) {
    /* try to print the object type, or object address, or a placeholder. */
    if (lst->objs[i] && lst->objs[i]->type)
      printf("%s", lst->objs[i]->type->name);
    else if (lst->objs[i])
      printf("0x%lx", (unsigned long) lst->objs[i]);
    else
      printf("[]");

    /* print a separator. */
    if (i < lst->n - 1)
      printf(", ");
  }

  /* end the list. */
  printf("}\n");
  return 1;
}

/* ObjectList_type: object type structure for matte object lists.
 */
struct _ObjectType ObjectList_type = {
  "ObjectList",                                  /* name       */
  sizeof(struct _ObjectList),                    /* size       */
  0,                                             /* precedence */

  (obj_constructor) object_list_new,             /* fn_new    */
  NULL,                                          /* fn_copy   */
  (obj_destructor)  object_list_delete,          /* fn_delete */
  (obj_display)     object_list_disp,            /* fn_disp   */

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

