
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the exceptions header. */
#include <matte/except.h>

/* include other required object headers. */
#include <matte/object-list.h>
#include <matte/string.h>

/* except_type(): return a pointer to the exception object type.
 */
ObjectType except_type (void) {
  /* return the exception address. */
  return &Exception_type;
}

/* except_new(): allocate a new matte exception.
 *
 * returns:
 *  newly allocated exception.
 */
Exception except_new (Object args) {
  /* allocate a new exception. */
  Exception e = (Exception) Exception_type.fn_alloc(&Exception_type);
  if (!e)
    return NULL;

  /* initialize the identifier and message strings. */
  e->id = string_new(NULL);
  e->msg = string_new(NULL);

  /* initialize the call stack. */
  e->stack = NULL;
  e->n_stack = 0;

  /* initialize the cause array. */
  e->cause = NULL;
  e->n_cause = 0;

  /* check if input arguments were provided. */
  if (args) {
    /* get the number of input arguments. */
    const int nargs = object_list_get_length((ObjectList) args);

    /* check if an identifier was provided. */
    if (nargs >= 1) {
      /* set the identifier. */
      Object eid = object_list_get((ObjectList) args, 0);
      if (IS_STRING(eid))
        string_append(e->id, (String) eid);
    }

    /* check if a message string was provided. */
    if (nargs >= 2) {
      /* set the message. */
      Object emsg = object_list_get((ObjectList) args, 1);
      if (IS_STRING(emsg))
        string_append(e->msg, (String) emsg);
    }

    /* check if extra variables were provided. */
    if (nargs >= 3) {
      /* construct a formatted message. */
      String emsg = string_new(NULL);
      string_append_objs(emsg, e->msg->data, 2, (ObjectList) args);

      /* set the formatted message. */
      object_free((Object) e->msg);
      e->msg = emsg;
    }
  }

  /* return the new exception. */
  return e;
}

/* except_free(): free all memory associated with a matte exception.
 *
 * arguments:
 *  @e: matte exception to free.
 */
void except_free (Exception e) {
  /* return if the exception is null. */
  if (!e)
    return;

  /* free the identifier and message strings. */
  object_free((Object) e->id);
  object_free((Object) e->msg);

  /* free the elements in the call stack. */
  for (long i = 0; i < e->n_stack; i++) {
    /* free the string data. */
    object_free((Object) e->stack[i].fname);
    object_free((Object) e->stack[i].func);
  }

  /* free the elements in the cause array. */
  for (long i = 0; i < e->n_cause; i++)
    object_free((Object) e->cause[i]);

  /* free the stack and cause arrays. */
  free(e->stack);
  free(e->cause);
}

/* except_disp(): display function for exceptions.
 */
int except_disp (Exception e, const char *var) {
  /* FIXME: implement except_disp() */

  /* return success. */
  return 1;
}

Object except_addCause (Exception e, ObjectList args) {
  /* FIXME: implement except_addCause() */

  /* return nothing. */
  return NULL;
}

/* except_methods: method table structure for matte exceptions.
 */
struct _ObjectMethod except_methods[] = {
  { "addCause", (obj_method) except_addCause },
  { NULL,       NULL                         }
};

/* Exception_type: object type structure for matte exceptions.
 */
struct _ObjectType Exception_type = {
  "Exception",                                   /* name       */
  sizeof(struct _Exception),                     /* size       */
  0,                                             /* precedence */

  (obj_constructor) except_new,                  /* fn_new     */
  NULL,                                          /* fn_copy    */
  (obj_allocator)   object_alloc,                /* fn_alloc   */
  (obj_destructor)  except_free,                 /* fn_dealloc */
  (obj_display)     except_disp,                 /* fn_disp    */

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

  except_methods                                 /* methods */
};

