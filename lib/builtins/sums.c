
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

Object matte_sum (Zone z, Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);
  Object x = object_list_get((ObjectList) argin, 0);
  Object dim = object_list_get((ObjectList) argin, 1);

  Object y = NULL;

  if (nargin == 1) {
    if (IS_INT(x))
      y = (Object) int_copy(z, (Int) x);
    else if (IS_FLOAT(x))
      y = (Object) float_copy(z, (Float) x);
    else if (IS_COMPLEX(x))
      y = (Object) complex_copy(z, (Complex) x);
    else if (IS_RANGE(x)) {
      Range r = (Range) x;
      long sum = 0;

      for (long elem = r->begin; elem <= r->end; elem += r->step)
        sum += elem;

      y = (Object) int_new_with_value(z, sum);
    }
    else if (IS_VECTOR(x)) {
      Vector v = (Vector) x;
      double sum = 0.0;

      for (long i = 0; i < v->n; i++)
        sum += v->data[i];

      y = (Object) float_new_with_value(z, sum);
    }
    else if (IS_COMPLEX_VECTOR(x)) {
      ComplexVector cv = (ComplexVector) x;
      complex double sum = 0.0;

      for (long i = 0; i < cv->n; i++)
        sum += cv->data[i];

      y = (Object) complex_new_with_value(z, sum);
    }
  }
  else if (nargin == 2) {
  }
  else
    throw(z, ERR_INVALID_ARGIN);

  return object_list_argout(z, 1, y);
}

Object matte_prod (Zone z, Object argin) {
  const int nargin = object_list_get_length((ObjectList) argin);
  Object x = object_list_get((ObjectList) argin, 0);
  Object dim = object_list_get((ObjectList) argin, 1);

  Object y = NULL;

  if (nargin == 1) {
    if (IS_INT(x))
      y = (Object) int_copy(z, (Int) x);
    else if (IS_FLOAT(x))
      y = (Object) float_copy(z, (Float) x);
    else if (IS_COMPLEX(x))
      y = (Object) complex_copy(z, (Complex) x);
    else if (IS_RANGE(x)) {
      Range r = (Range) x;
      long prod = 1;

      for (long elem = r->begin; elem <= r->end; elem += r->step)
        prod *= elem;

      y = (Object) int_new_with_value(z, prod);
    }
    else if (IS_VECTOR(x)) {
      Vector v = (Vector) x;
      double prod = 1.0;

      for (long i = 0; i < v->n; i++)
        prod *= v->data[i];

      y = (Object) float_new_with_value(z, prod);
    }
    else if (IS_COMPLEX_VECTOR(x)) {
      ComplexVector cv = (ComplexVector) x;
      complex double prod = 1.0;

      for (long i = 0; i < cv->n; i++)
        prod *= cv->data[i];

      y = (Object) complex_new_with_value(z, prod);
    }
  }
  else if (nargin == 2) {
  }
  else
    throw(z, ERR_INVALID_ARGIN);

  return object_list_argout(z, 1, y);
}

