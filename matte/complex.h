
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_COMPLEX_H__
#define __MATTE_COMPLEX_H__

/* include the object header. */
#include <matte/object.h>

/* IS_COMPLEX: macro to check that an object is a matte complex float.
 */
#define IS_COMPLEX(obj) \
  MATTE_TYPE_CHECK(obj, &Complex_type)

/* Complex: pointer to a struct _ComplexFloat. */
typedef struct _ComplexFloat *Complex;
struct _ObjectType Complex_type;

/* _ComplexFloat: structure for holding a complex float object.
 */
struct _ComplexFloat {
  /* base object. */
  OBJECT_BASE;

  /* @value: value of the complex float.
   */
  complex double value;
};

/* function declarations (complex.c): */

ObjectType complex_type (void);

Complex complex_new (Zone z, Object args);

Complex complex_new_with_value (Zone z, complex double value);

Complex complex_copy (Zone z, Complex f);

Complex complex_copyconj (Zone z, Complex f);

complex double complex_get_value (Complex f);

double complex_get_real (Complex f);

double complex_get_imag (Complex f);

double complex_get_abs (Complex f);

double complex_get_arg (Complex f);

void complex_set_value (Complex f, complex double value);

void complex_set_real (Complex f, double value);

void complex_set_imag (Complex f, double value);

#endif /* !__MATTE_COMPLEX_H__ */

