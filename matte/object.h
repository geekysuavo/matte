
/* Copyright (c) 2016, 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_OBJECT_H__
#define __MATTE_OBJECT_H__

/* include required standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/* include required c math library headers. */
#include <math.h>
#include <complex.h>

/* include the blas/lapack headers. */
#include <cblas.h>
#include <clapack.h>

/* include the required matte headers. */
#include <matte/zone.h>

/* type definitions of blas/lapack enumerations:
 *  MatteTranspose: transpose options.
 *  MatteTriangle: triangle options.
 *  MatteDiagonal: diagonal options.
 */
typedef enum CBLAS_TRANSPOSE MatteTranspose;
typedef enum CBLAS_UPLO MatteTriangle;
typedef enum CBLAS_DIAG MatteDiagonal;

/* MATTE_TYPE: macro to obtain the type structure pointer of an object.
 */
#define MATTE_TYPE(obj) \
  (((Object) obj)->type)

/* MATTE_TYPE_CHECK: macro to check that a matte object has a specific type.
 */
#define MATTE_TYPE_CHECK(obj,typ) \
  ((obj) && MATTE_TYPE(obj) == typ)

/* OBJECT_BASE: macro to place at the beginning of every matte object.
 * this allows all objects to be directly cast into the base object
 * type for memory management, dynamic dispatch, etc.
 */
#define OBJECT_BASE \
  Object base;

/* ObjectType: pointer to a struct _ObjectType. */
typedef struct _ObjectType *ObjectType;

/* Object: pointer to a struct _Object. */
typedef struct _Object *Object;

/* ObjectMethods: pointer to a struct _ObjectMethod. */
typedef struct _ObjectMethod *ObjectMethods;

/* general-purpose function pointer type definition:
 */
typedef Object (*matte_func) (Zone, Object);

/* function pointer type definitions for matte objects:
 */
typedef Object (*obj_constructor) (Zone, Object);
typedef void   (*obj_destructor)  (Zone, Object);
typedef int    (*obj_display)     (Zone, Object);
typedef int    (*obj_assert)      (Object);
typedef Object (*obj_unary)       (Zone, Object);
typedef Object (*obj_binary)      (Zone, Object, Object);
typedef Object (*obj_ternary)     (Zone, Object, Object, Object);
typedef Object (*obj_variadic)    (Zone, int, va_list);
typedef Object (*obj_method)      (Zone, Object, Object);

/* _ObjectMethod: structure that holds information about a matte object
 * method.
 */
struct _ObjectMethod {
  /* @name: string name of the method.
   * @fn: method function pointer.
   */
  const char *name;
  obj_method fn;
};

/* _ObjectType: structure that holds the core set of information required
 * by the matte object system.
 */
struct _ObjectType {
  /* core object information:
   *  @name: string name of the matte type.
   *  @size: base size of the type struct.
   *  @precedence: object precedence for dynamic dispatch.
   */
  const char *name;
  unsigned int size;
  unsigned int precedence;

  /* core object method table:
   */
  obj_constructor fn_new;
  obj_constructor fn_copy;
  obj_destructor  fn_delete;
  obj_display     fn_disp;
  obj_assert      fn_true;

  /* numeric method table:
   */
  obj_binary   fn_plus;        /* a+b      binary addition.             */
  obj_binary   fn_minus;       /* a-b      binary subtraction.          */
  obj_unary    fn_uminus;      /* -a       unary minus.                 */
  obj_binary   fn_times;       /* a.*b     element-wise multiplication. */
  obj_binary   fn_mtimes;      /* a*b      matrix multiplication.       */
  obj_binary   fn_rdivide;     /* a./b     element-wise right division. */
  obj_binary   fn_ldivide;     /* a.\b     element-wise left division.  */
  obj_binary   fn_mrdivide;    /* a/b      matrix right division.       */
  obj_binary   fn_mldivide;    /* a\b      matrix left division.        */
  obj_binary   fn_power;       /* a.^b     element-wise power.          */
  obj_binary   fn_mpower;      /* a^b      matrix power.                */
  obj_binary   fn_lt;          /* a<b      less than.                   */
  obj_binary   fn_gt;          /* a>b      greater than.                */
  obj_binary   fn_le;          /* a<=b     less than or equal to.       */
  obj_binary   fn_ge;          /* a>=b     greater than or equal to.    */
  obj_binary   fn_ne;          /* a!=b     inequality.                  */
  obj_binary   fn_eq;          /* a==b     equality.                    */
  obj_binary   fn_and;         /* a&b      logical and.                 */
  obj_binary   fn_or;          /* a|b      logical or.                  */
  obj_binary   fn_mand;        /* a&&b     matrix logical and.          */
  obj_binary   fn_mor;         /* a||b     matrix logical or.           */
  obj_unary    fn_not;         /* !a       logical negation.            */
  obj_ternary  fn_colon;       /* a:d:b    colon operator.              */
  obj_unary    fn_ctranspose;  /* a'       conjugate transpose.         */
  obj_unary    fn_transpose;   /* a.'      matrix transpose.            */
  obj_variadic fn_horzcat;     /* [a,b]    horizontal concatenation.    */
  obj_variadic fn_vertcat;     /* [a;b]    vertical concatenation.      */
  obj_binary   fn_subsref;     /* a(s)     subscripted reference.       */
  obj_ternary  fn_subsasgn;    /* a(s)=b   subscripted assignment.      */
  obj_unary    fn_subsindex;   /* b(a)     subscript index.             */

  /* general-purpose method table:
   */
  ObjectMethods methods;
};

/* _Object: structure for type-casting between any number of matte objects.
 * holds a pointer to the object type structure as its first element.
 */
struct _Object {
  /* @type: the type of matte object that is located
   * at the current address in memory.
   */
  ObjectType type;

  /* in all other matte objects, object instance variables go here. */
};

/* function declarations (object.c): */

Object object_alloc (Zone z, ObjectType type);

Object object_copy (Zone z, Object obj);

void object_free (Zone z, void *ptr);

void object_free_all (Zone z);

int object_disp (Zone z, Object obj);

int object_display (Zone z, Object obj, const char *var);

int object_true (Object obj);

/* object method declarations (object.c): */

Object object_plus       (Zone z, Object a, Object b);
Object object_minus      (Zone z, Object a, Object b);
Object object_uminus     (Zone z, Object a);
Object object_times      (Zone z, Object a, Object b);
Object object_mtimes     (Zone z, Object a, Object b);
Object object_rdivide    (Zone z, Object a, Object b);
Object object_ldivide    (Zone z, Object a, Object b);
Object object_mrdivide   (Zone z, Object a, Object b);
Object object_mldivide   (Zone z, Object a, Object b);
Object object_power      (Zone z, Object a, Object b);
Object object_mpower     (Zone z, Object a, Object b);
Object object_lt         (Zone z, Object a, Object b);
Object object_gt         (Zone z, Object a, Object b);
Object object_le         (Zone z, Object a, Object b);
Object object_ge         (Zone z, Object a, Object b);
Object object_ne         (Zone z, Object a, Object b);
Object object_eq         (Zone z, Object a, Object b);
Object object_and        (Zone z, Object a, Object b);
Object object_or         (Zone z, Object a, Object b);
Object object_mand       (Zone z, Object a, Object b);
Object object_mor        (Zone z, Object a, Object b);
Object object_not        (Zone z, Object a);
Object object_colon      (Zone z, Object a, Object b, Object c);
Object object_ctranspose (Zone z, Object a);
Object object_transpose  (Zone z, Object a);
Object object_horzcat    (Zone z, int n, ...);
Object object_vertcat    (Zone z, int n, ...);
Object object_subsref    (Zone z, Object a, Object b);
Object object_subsasgn   (Zone z, Object a, Object b, Object c);
Object object_subsindex  (Zone z, Object a);

/* utility function declarations: */

char *strdup (const char *s);

#endif /* !__MATTE_OBJECT_H__ */

