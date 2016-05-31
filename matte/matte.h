
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_MATTE_H__
#define __MATTE_MATTE_H__

/* include required standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include the c math library headers. */
#include <math.h>
#include <complex.h>

/* include the matte core object headers. */
#include <matte/object.h>
#include <matte/except.h>
#include <matte/object-list.h>
#include <matte/iter.h>

/* include the matte non-numeric object headers. */
#include <matte/cell.h>
#include <matte/struct.h>
#include <matte/string.h>

/* include the matte numeric object headers. */
#include <matte/int.h>
#include <matte/range.h>
#include <matte/float.h>
#include <matte/complex.h>
#include <matte/vector.h>
#include <matte/matrix.h>
#include <matte/complex-vector.h>
#include <matte/complex-matrix.h>

/* include the matte builtin function header. */
#include <matte/builtins.h>

#endif /* !__MATTE_MATTE_H__ */

