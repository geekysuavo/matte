
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_ERRORS_H__
#define __MATTE_ERRORS_H__

/* include required c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* error(): macro function to add an error message into the global error
 * message array.
 */
#define error(...) \
  matte_errors_add(__FILE__, __LINE__, __VA_ARGS__)

/* fail(): macro function to add an error message into the global array,
 * then return an integer indicating failure (0).
 */
#define fail(...) \
  { matte_errors_add(__FILE__, __LINE__, __VA_ARGS__); \
    return 0; }

/* throw(): macro function to add an error message into the global array,
 * then return a pointer indicating failure (0).
 */
#define throw(...) \
  { matte_errors_add(__FILE__, __LINE__, __VA_ARGS__); \
    return NULL; }

/* die(): macro function to add an error message into the global array,
 * then print the error array contents and end program execution.
 */
#define die(...) \
  { matte_errors_add(__FILE__, __LINE__, __VA_ARGS__); \
    matte_errors_print(); \
    return 1; }

/* function declarations (errors.c): */

void matte_errors_init (void);

void matte_errors_print (void);

void matte_errors_reset (void);

void matte_errors_add (const char *fname, unsigned int line,
                       const char *format, ...);

#endif /* !__MATTE_ERRORS_H__ */

