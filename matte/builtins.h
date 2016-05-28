
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_BUILTINS_H__
#define __MATTE_BUILTINS_H__

/* include the object and symbol table headers. */
#include <matte/object.h>
#include <matte/symbols.h>

/* core function declarations (builtins.c): */

int matte_globals_init (Symbols gs);

int matte_builtins_init (Symbols gs);

/* built-in function declarations (builtins.c): */

Object matte_sum (Zone z, Object argin);

Object matte_sprintf (Zone z, Object argin);

#endif /* !__MATTE_BUILTINS_H__ */

