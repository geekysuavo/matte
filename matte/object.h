
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_OBJECT_H__
#define __MATTE_OBJECT_H__

/* include required standard c library headers. */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/* include required c math library headers. */
#include <math.h>
#include <complex.h>

/* include the blas/lapack headers. */
#include <cblas.h>
#include <clapack.h>

/* include the require matte headers. */
#include <matte/errors.h>
#include <matte/zone.h>

/* MatteTranspose: type definition of blas/lapack transpose options.
 */
typedef enum CBLAS_TRANSPOSE MatteTranspose;

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
typedef Object (*matte_func) (Object);

/* function pointer type definitions for matte objects:
 */
typedef Object (*obj_allocator)   (ObjectType);
typedef Object (*obj_constructor) (Object);
typedef void   (*obj_destructor)  (Object);
typedef int    (*obj_display)     (Object, const char *);
typedef Object (*obj_unary)       (Object);
typedef Object (*obj_binary)      (Object, Object);
typedef Object (*obj_ternary)     (Object, Object, Object);
typedef Object (*obj_variadic)    (int, va_list);
typedef Object (*obj_method)      (Object, Object);

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
  obj_allocator   fn_alloc;
  obj_destructor  fn_dealloc;
  obj_display     fn_disp;

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

Object object_alloc (ObjectType type);

Object object_new (ObjectType type, Object args);

void object_free (Object obj);

int object_disp (Object obj, const char *var);

/* object method declarations (object.c): */

Object object_plus       (Object a, Object b);
Object object_minus      (Object a, Object b);
Object object_uminus     (Object a);
Object object_times      (Object a, Object b);
Object object_mtimes     (Object a, Object b);
Object object_rdivide    (Object a, Object b);
Object object_ldivide    (Object a, Object b);
Object object_mrdivide   (Object a, Object b);
Object object_mldivide   (Object a, Object b);
Object object_power      (Object a, Object b);
Object object_mpower     (Object a, Object b);
Object object_lt         (Object a, Object b);
Object object_gt         (Object a, Object b);
Object object_le         (Object a, Object b);
Object object_ge         (Object a, Object b);
Object object_ne         (Object a, Object b);
Object object_eq         (Object a, Object b);
Object object_and        (Object a, Object b);
Object object_or         (Object a, Object b);
Object object_mand       (Object a, Object b);
Object object_mor        (Object a, Object b);
Object object_not        (Object a);
Object object_colon      (Object a, Object b, Object c);
Object object_ctranspose (Object a);
Object object_transpose  (Object a);
Object object_horzcat    (int n, ...);
Object object_vertcat    (int n, ...);
Object object_subsref    (Object a, Object b);
Object object_subsasgn   (Object a, Object b, Object c);
Object object_subsindex  (Object a);

/* utility function declarations: */

char *strdup (const char *s);

#endif /* !__MATTE_OBJECT_H__ */

