
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_CELL_H__
#define __MATTE_CELL_H__

/* include the object header. */
#include <matte/object.h>

/* IS_CELL: macro to check if an object is a matte cell array.
 */
#define IS_CELL(obj) \
  MATTE_TYPE_CHECK(obj, cell_type())

/* Cell: pointer to a struct _Cell. */
typedef struct _Cell *Cell;
struct _ObjectType Cell_type;

/* _Cell: structure for holding a vector or matrix of matte objects.
 */
struct _Cell {
  /* base object. */
  OBJECT_BASE;

  /* @data: array of pointers to matte objects.
   * @m: number of array rows.
   * @n: number of array columns.
   */
  Object *data;
  long m, n;

  /* @tr: transposition status of the array.
   */
  MatteTranspose tr;
};

/* function declarations (cell.c): */

ObjectType cell_type (void);

Cell cell_new (Zone z, Object args);

Cell cell_new_with_size (Zone z, long m, long n);

Cell cell_copy (Zone z, Cell arr);

void cell_delete (Zone z, Cell arr);

long cell_get_rows (Cell arr);

long cell_get_cols (Cell arr);

long cell_get_length (Cell arr);

#endif /* !__MATTE_CELL_H__ */

