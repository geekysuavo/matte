
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_BUILTINS_H__
#define __MATTE_BUILTINS_H__

/* include the object and symbol table headers. */
#include <matte/object.h>
#include <matte/symbols.h>

/* function declarations (builtins.c): */

int matte_builtins_init (Symbols gs);

Object matte_sum (Object argin);

#endif /* !__MATTE_BUILTINS_H__ */

