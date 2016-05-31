
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_EXCEPT_H__
#define __MATTE_EXCEPT_H__

/* include the object header. */
#include <matte/object.h>

/* include the string and integer headers. */
#include <matte/string.h>
#include <matte/int.h>

/* include the matte exception definition header. */
#include <matte/except-defs.h>

/* IS_EXCEPTION: macro to check that an object is a matte exception.
 */
#define IS_EXCEPTION(obj) \
  MATTE_TYPE_CHECK(obj, except_type())

/* Exception: pointer to a struct _Exception. */
typedef struct _Exception *Exception;
struct _ObjectType Exception_type;

/* ExceptionStack: pointer to a struct _ExceptionStack. */
typedef struct _ExceptionStack *ExceptionStack;

/* _ExceptionStack: structure for holing excpetion stack trace data.
 */
struct _ExceptionStack {
  /* @fname: name of the source file that threw the exception.
   * @func: name of the function that threw the exception.
   * @line: line in the input where the throw occurred.
   */
  String fname;
  String func;
  Int line;
};

/* _Exception: structure for holding exception information.
 */
struct _Exception {
  /* base object. */
  OBJECT_BASE;

  /* @id: unique error type identifier string.
   * @msg: error message string.
   */
  String id, msg;

  /* @stack: array of function calls that led to the exception.
   * @n_stack: number of function calls in the call stack.
   */
  ExceptionStack stack;
  long n_stack;

  /* @cause: array of secondary exceptions that cause the exception.
   * @n_cause: number of secondary exceptions in the cause array.
   */
  Exception *cause;
  long n_cause;
};

/* global exception function declarations (except.c): */

void exceptions_add (const char *fname, const char *func,
                     unsigned long line, const char *id,
                     const char *format, ...);

Object exceptions_get (Zone z);

int exceptions_check (void);

void exceptions_disp (void);

/* object function declarations (except.c): */

ObjectType except_type (void);

Exception except_new (Zone z, Object args);

Exception except_copy (Zone z, Exception e);

void except_delete (Zone z, Exception e);

int except_set_strings (Zone z, Exception e, const char *id,
                        const char *msg);

int except_add_call (Zone z, Exception e, const char *fname,
                     const char *func, long line);

int except_add_cause (Zone z, Exception e, Exception esub);

#endif /* !__MATTE_EXCEPT_H__ */

