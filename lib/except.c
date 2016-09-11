
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the exceptions header. */
#include <matte/except.h>

/* include other required object headers. */
#include <matte/object-list.h>
#include <matte/string.h>

/* exceptions: structure for holding information collected from calls
 * to the fail() macro function.
 */
static Exception exceptions = NULL;

/* exceptions_add(): append a call stack frame to the global exception.
 *
 * arguments:
 *  @fname: name of the source file that threw the exception.
 *  @func: name of the function that threw the exception.
 *  @line: line in the source file where the exception was thrown.
 *  @id: identifier string to store for the exception.
 *  @format: printf-style format string for custom error messages.
 *  @...: arguments that accompany the format string.
 */
void exceptions_add (const char *fname, const char *func,
                     unsigned long line, const char *id,
                     const char *format, ...) {
  /* declare required variables:
   *  @vl: variable-length argument list for string formatting.
   */
  char msg[1024];
  va_list vl;

  /* ensure the global exceptions object is allocated. */
  if (!exceptions) {
    /* perform the initial allocation. */
    exceptions = except_new(NULL, NULL);
    if (!exceptions)
      return;

    /* build the message string. */
    va_start(vl, format);
    vsnprintf(msg, 1024, format, vl);
    va_end(vl);

    /* store the exception strings. */
    except_set_strings(NULL, exceptions, id, msg);
  }

  /* add the call information into the call stack. */
  except_add_call(NULL, exceptions, fname, func, line);
}

/* exceptions_get(): return a copy of the global exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *
 * returns:
 *  copy of the current state of the global exception object.
 */
Object exceptions_get (Zone z) {
  /* copy the exception object into the zone allocator. */
  return (Object) except_copy(z, exceptions);
}

/* exceptions_check(): check if any exceptions have occurred.
 */
int exceptions_check (void) {
  /* return whether or not the global exception has been allocated. */
  return (exceptions ? 1 : 0);
}

/* exceptions_disp(): display the contents of the global exception.
 */
void exceptions_disp (void) {
  /* display the exception. */
  object_disp(NULL, (Object) exceptions);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* except_type(): return a pointer to the exception object type.
 */
ObjectType except_type (void) {
  /* return the exception address. */
  return &Exception_type;
}

/* except_new(): allocate a new matte exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated exception.
 */
Exception except_new (Zone z, Object args) {
  /* allocate a new exception. */
  Exception e = (Exception) object_alloc(z, &Exception_type);
  if (!e)
    return NULL;

  /* initialize the identifier and message strings. */
  e->id = string_new(z, NULL);
  e->msg = string_new(z, NULL);

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
      String emsg = string_new(z, NULL);
      string_append_objs(emsg, e->msg->data, 2, (ObjectList) args);

      /* set the formatted message. */
      object_free(z, e->msg);
      e->msg = emsg;
    }
  }

  /* return the new exception. */
  return e;
}

/* except_copy(): allocate a new matte exception from another
 * matte exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @e: matte exception to duplicate.
 *
 * returns:
 *  duplicated matte exception.
 */
Exception except_copy (Zone z, Exception e) {
  /* return null if the input argument is null. */
  if (!e)
    return NULL;

  /* allocate a new exception. */
  Exception enew = except_new(z, NULL);
  if (!enew)
    return NULL;

  /* copy the exception strings. */
  except_set_strings(z, enew, string_get_value(e->id),
                              string_get_value(e->msg));

  /* copy the call stack entries. */
  for (long i = 0; i < e->n_stack; i++)
    except_add_call(z, enew, string_get_value(e->stack[i].fname),
                             string_get_value(e->stack[i].func),
                             int_get_value(e->stack[i].line));

  /* copy the secondary exceptions. */
  for (long i = 0; i < e->n_cause; i++)
    except_add_cause(z, enew, e->cause[i]);

  /* return the new exception. */
  return enew;
}

/* except_delete(): free all memory associated with a matte exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @e: matte exception to free.
 */
void except_delete (Zone z, Exception e) {
  /* return if the exception is null. */
  if (!e)
    return;

  /* free the identifier and message strings. */
  object_free(z, e->id);
  object_free(z, e->msg);

  /* free the elements in the call stack. */
  for (long i = 0; i < e->n_stack; i++) {
    /* free the string data. */
    object_free(z, e->stack[i].fname);
    object_free(z, e->stack[i].func);
  }

  /* free the elements in the cause array. */
  for (long i = 0; i < e->n_cause; i++)
    object_free(z, e->cause[i]);

  /* free the stack and cause arrays. */
  free(e->stack);
  free(e->cause);
}

/* except_set_strings(): set the identifier and message string values
 * of a matte exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @e: matte exception to modify.
 *  @id: identifier string value.
 *  @msg: message string value.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int except_set_strings (Zone z, Exception e, const char *id,
                        const char *msg) {
  /* return if the exception is null. */
  if (!e)
    return 0;

  /* set the string values. */
  if (!string_set_value(e->id, id) || !string_set_value(e->msg, msg))
    return 0;

  /* return success. */
  return 1;
}

/* except_add_call(): add an entry of call information into the call stack
 * of a matte exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @e: matte exception to modify.
 *  @fname: filename string to add.
 *  @func: function string to add.
 *  @line: line number to add.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int except_add_call (Zone z, Exception e, const char *fname,
                     const char *func, long line) {
  /* return if the exception is null. */
  if (!e)
    return 0;

  /* resize the call stack. */
  e->n_stack++;
  e->stack = (ExceptionStack)
    realloc(e->stack, e->n_stack * sizeof(struct _ExceptionStack));

  /* check if reallocation failed. */
  if (!e->stack)
    return 0;

  /* copy the call information into the call stack. */
  e->stack[e->n_stack - 1].fname = string_new_with_value(z, fname);
  e->stack[e->n_stack - 1].func  = string_new_with_value(z, func);
  e->stack[e->n_stack - 1].line  = int_new_with_value(z, line);

  /* return success. */
  return 1;
}

/* except_add_cause(): add an exception object into the cause array
 * of a matte exception.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @e: matte exception to modify.
 *  @esub: cause exception to add.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int except_add_cause (Zone z, Exception e, Exception esub) {
  /* return if either exception is null. */
  if (!e || !esub)
    return 0;

  /* resize the cause array. */
  e->n_cause++;
  e->cause = (Exception*)
    realloc(e->cause, e->n_cause * sizeof(Exception));

  /* check if reallocation failed. */
  if (!e->cause)
    return 0;

  /* copy the new secondary exception into the cause array. */
  e->cause[e->n_cause - 1] = except_copy(z, esub);

  /* return success. */
  return 1;
}

/* Exception.addCause(): add a secondary cause to an exception.
 */
Object except_addCause (Zone z, Exception e, ObjectList args) {
  /* get the number of input arguments. */
  const int nargin = object_list_get_length(args);

  /* only accept a single input argument. */
  if (nargin == 1) {
    /* get the input arguments. */
    Object obj = object_list_get(args, 0);

    /* add the exception object to the cause array. */
    if (IS_EXCEPTION(obj) && except_add_cause(z, e, (Exception) obj))
      return (Object) e;
  }

  /* return nothing. */
  throw(z, ERR_INVALID_ARGIN);
}

/* except_disp(): display function for exceptions.
 */
int except_disp (Zone z, Exception e) {
  /* print the main error message. */
  printf(ANSI_RED "error:" ANSI_NORM " "
         ANSI_BOLD "%s:" ANSI_NORM "\n %s\n\n",
         string_get_value(e->id), string_get_value(e->msg));

  /* print the call stack. */
  printf("call stack:\n");
  for (long i = 0; i < e->n_stack; i++) {
    printf(" [%ld] " ANSI_BOLD "%s:%ld" ANSI_NORM
           " within function '" ANSI_BOLD "%s" ANSI_NORM "'\n", i,
           string_get_value(e->stack[i].fname),
           int_get_value(e->stack[i].line),
           string_get_value(e->stack[i].func));
  }

  /* return success. */
  printf("\n");
  return 1;
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

  (obj_constructor) except_new,                  /* fn_new    */
  (obj_constructor) except_copy,                 /* fn_copy   */
  (obj_destructor)  except_delete,               /* fn_delete */
  (obj_display)     except_disp,                 /* fn_disp   */
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

  except_methods                                 /* methods */
};

