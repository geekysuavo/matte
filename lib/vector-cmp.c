
/* define macros for generating a vector comparison method.
 */
#define CONCAT(a,b) a ## b
#define FUNCTION(name) CONCAT(vector_, name)

Object FUNCTION(F) (Zone z, Object a, Object b) {
  if (IS_VECTOR(a)) {
    if (IS_VECTOR(b)) {
      Vector va = (Vector) a;
      Vector vb = (Vector) b;

      if (va->tr == vb->tr) {
        /* column <op> column => column
         * row    <op> row    => row
         */
        if (va->n != vb->n)
          throw(z, ERR_SIZE_MISMATCH_VV(va, vb));

        Vector vc = vector_new_with_length(z, va->n);
        vc->tr = va->tr;
        if (vc) {
          for (long i = 0; i < vc->n; i++)
            vector_set(vc, i, vector_get(va, i) OP vector_get(vb, i));

          return (Object) vc;
        }
      }
      else if (va->tr == CblasNoTrans) {
        /* column <op> row => matrix */
        Matrix A = matrix_new_with_size(z, va->n, vb->n);
        if (A) {
          for (long i = 0; i < A->m; i++)
            for (long j = 0; j < A->n; j++)
              matrix_set(A, i, j, vector_get(va, i) OP vector_get(vb, j));

          return (Object) A;
        }
      }

      return NULL;
    }

    /* vector <op> complex => vector
     * vector <op> float   => vector
     * vector <op> int     => vector
     */
    Vector v = vector_copy(z, (Vector) a);
    if (!v)
      return NULL;

    double fval;
    if (IS_COMPLEX(b)) {
#ifdef CMPEQ
      const complex double bval = complex_get_value((Complex) b);
      fval = (cimag(bval) ? NAN : creal(bval));
#else
      fval = complex_get_abs((Complex) b);
#endif
    }
    else if (IS_FLOAT(b))
      fval = float_get_value((Float) b);
    else if (IS_INT(b))
      fval = (double) int_get_value((Int) b);
    else
      return NULL;

    for (long i = 0; i < v->n; i++)
      vector_set(v, i, vector_get(v, i) OP fval);

    return (Object) v;
  }
  else if (IS_VECTOR(b)) {
    /* complex <op> vector => vector
     * float   <op> vector => vector
     * int     <op> vector => vector
     */
    Vector v = vector_copy(z, (Vector) b);
    if (!v)
      return NULL;

    double fval;
    if (IS_COMPLEX(a)) {
#ifdef CMPEQ
      const complex double aval = complex_get_value((Complex) a);
      fval = (cimag(aval) ? NAN : creal(aval));
#else
      fval = complex_get_abs((Complex) a);
#endif
    }
    else if (IS_FLOAT(b))
      fval = float_get_value((Float) a);
    else if (IS_INT(a))
      fval = (double) int_get_value((Int) a);
    else
      return NULL;

    for (long i = 0; i < v->n; i++)
      vector_set(v, i, fval OP vector_get(v, i));

    return (Object) v;
  }

  return NULL;
}

/* undefine the method generator macros.
 */
#undef CONCAT
#undef FUNCTION
#undef CMPEQ
#undef OP
#undef F

