
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

/* exception definitions: */

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

#define ERR_UNDEFINED_SYMBOL \
  "matte:compiler", "symbol '" ANSI_BOLD "%s" ANSI_NORM "' is undefined"

#endif /* !__MATTE_EXCEPT_DEFS_H__ */

