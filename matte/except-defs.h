
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_EXCEPT_DEFS_H__
#define __MATTE_EXCEPT_DEFS_H__

/* definitions of useful ansi terminal codes for generating pretty text.
 */
#define ANSI_NORM "\x1B[0m"
#define ANSI_BOLD "\x1B[01m"
#define ANSI_RED  "\x1B[01;31m"
#define ANSI_MAG  "\x1B[01;35m"

/* warning definitions: */

#define WARN_OBJ_COPY \
  "copied object has no copy constructor"

#define WARN_OBJ_TRUE \
  "object of type '" ANSI_BOLD "%s" ANSI_NORM "' used as condition"

/* error definitions: */

#define ERR_FOPEN \
  "matte:invalid-file", \
  "unable to open '" ANSI_BOLD "%s" ANSI_NORM "' for reading"

#define ERR_DLOPEN \
  "matte:invalid-file", \
  "unable to open '" ANSI_BOLD "%s" ANSI_NORM "' into memory"

#define ERR_BAD_ALLOC \
  "matte:bad-alloc", "allocation failed"

#define ERR_OBJ_ALLOC \
  "matte:bad-alloc", \
  "failed to allocate object of type '" ANSI_BOLD "%s" ANSI_NORM "'"

#define ERR_OBJ_UNARY \
  "matte:undefined-function", \
  "method " ANSI_BOLD "%s" ANSI_NORM "(" ANSI_BOLD "%s" ANSI_NORM ")" \
  " is unimplemented" 

#define ERR_OBJ_BINARY \
  "matte:undefined-function", \
  "method " ANSI_BOLD "%s" ANSI_NORM "(" \
  ANSI_BOLD "%s" ANSI_NORM ", " ANSI_BOLD "%s" ANSI_NORM ")" \
  " is unimplemented" 

#define ERR_OBJ_TERNARY \
  "matte:undefined-function", \
  "method " ANSI_BOLD "%s" ANSI_NORM "(" \
  ANSI_BOLD "%s" ANSI_NORM ", " \
  ANSI_BOLD "%s" ANSI_NORM ", " \
  ANSI_BOLD "%s" ANSI_NORM ")" \
  " is unimplemented" 

#define ERR_OBJ_VARIADIC \
  "matte:undefined-function", \
  "method " ANSI_BOLD "%s" ANSI_NORM "(" \
  ANSI_BOLD "%s" ANSI_NORM ", " \
  ANSI_BOLD "..." ANSI_NORM ")" \
  " is unimplemented" 

#define ERR_SIZE_MISMATCH \
  "matte:size-mismatch", "operand sizes do not match"

#define ERR_INVALID_ARGIN \
  "matte:invalid-input-arg", "one or more invalid arguments"

#define ERR_OUTSIDE_LOOP \
  "matte:compiler", "found '" ANSI_BOLD "%s" ANSI_NORM "' outside of a loop"

#define ERR_ITER_SUPPORT \
  "matte:iterator", \
  "iteration on '" ANSI_BOLD "%s" ANSI_NORM "' is not supported"

#define ERR_UNDEFINED_SYMBOL \
  "matte:compiler", "symbol '" ANSI_BOLD "%s" ANSI_NORM "' is undefined"

#define ERR_COMPILER_GENERAL \
  "matte:compiler", "compilation errors occured"

#define ERR_COMPILER_EMPTY \
  "matte:compiler", "no input files supplied"

#define ERR_COMPILER_MODE \
  "matte:compiler", "unsupported compilation mode"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* warn(): macro function to print a warning message.
 */
#define warn(...) \
  { printf(ANSI_MAG "warning:" ANSI_NORM " " \
           ANSI_BOLD "matte:runtime:" ANSI_NORM "\n " __VA_ARGS__); \
    printf("\n\n"); }

/* error(): macro function to add an error message into the global
 * exception object.
 */
#define error(...) \
  exceptions_add(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);

/* fail(): macro function to add an error message into the global
 * exception object, then return zero.
 */
#define fail(...) \
  { exceptions_add(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    return 0; }

/* throw(): macro function to add an error message into the global
 * exception object, copy the object into a zone allocator, and
 * return the copied exception.
 */
#define throw(z, ...) \
  { exceptions_add(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    return exceptions_get(z); }

/* die(): macro function to add an error message into the global
 * exception object, print the contents of the object, and return
 * an error status code for main().
 */
#define die(...) \
  { exceptions_add(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    exceptions_disp(); return 1; }

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* EXCEPT_PROPAGATE: macro function used to propagate an exception from
 * any function after receiving an exception.
 */
#define EXCEPT_PROPAGATE(var, fi, fn, ln) \
  { Exception _e = except_copy(_z0, (Exception) var); \
    except_add_call(_z0, _e, fi, fn, ln); \
    return (Object) _e; }

/* EXCEPT_HANDLE: macro function to handle exceptions within any function.
 */
#define EXCEPT_HANDLE(var, fi, fn, ln) \
  if (IS_EXCEPTION(var)) EXCEPT_PROPAGATE(var, fi, fn, ln)

#endif /* !__MATTE_EXCEPT_DEFS_H__ */

