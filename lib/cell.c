
/* Copyright (c) 2017 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the cell array and exception headers. */
#include <matte/cell.h>
#include <matte/except.h>

/* cell_type(): return a pointer to the cell array object type.
 */
ObjectType cell_type (void) {
  /* return the struct address. */
  return &Cell_type;
}

/* cell_new(): allocate a new matte cell array.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated empty cell array.
 */
Cell cell_new (Zone z, Object args) {
  /* allocate a new cell array. */
  Cell arr = (Cell) object_alloc(z, &Cell_type);
  if (!arr)
    return NULL;

  /* initialize the array data and size. */
  arr->data = NULL;
  arr->m = 0;
  arr->n = 0;

  /* initialize the array transposition state. */
  arr->tr = CblasNoTrans;

  /* return the new array. */
  return arr;
}

/* cell_delete(): free all memory associated with a matte cell array.
 *
 * arguments:
 *  @z: zone allocator to utilize.
 *  @arr: matte cell array to free.
 */
void cell_delete (Zone z, Cell arr) {
  /* return if the array is null. */
  if (!arr)
    return;

  /* free the array data. */
  free(arr->data);
}

/* Cell_type: object type structure for matte cell arrays.
 */
struct _ObjectType Cell_type = {
  "Cell",                                        /* name       */
  sizeof(struct _Cell),                          /* size       */
  0,                                             /* precedence */

  (obj_constructor) cell_new,                    /* fn_new    */
  NULL,                                          /* fn_copy   */
  (obj_destructor)  cell_delete,                 /* fn_delete */
  NULL,                                          /* fn_disp   */
  NULL,                                          /* fn_true   */

  NULL,                                          /* fn_plus       */
  NULL,                                          /* fn_minus      */
  NULL,                                          /* fn_uminus     */
  NULL,                                          /* fn_times      */
  NULL,                                          /* fn_mtimes     */
  NULL,                                          /* fn_rdivide    */
  NULL,                                          /* fn_ldivide    */
  NULL,                                          /* fn_mrdivide   */
  NULL,                                          /* fn_mldivide   */
  NULL,                                          /* fn_power      */
  NULL,                                          /* fn_mpower     */
  NULL,                                          /* fn_lt         */
  NULL,                                          /* fn_gt         */
  NULL,                                          /* fn_le         */
  NULL,                                          /* fn_ge         */
  NULL,                                          /* fn_ne         */
  NULL,                                          /* fn_eq         */
  NULL,                                          /* fn_and        */
  NULL,                                          /* fn_or         */
  NULL,                                          /* fn_mand       */
  NULL,                                          /* fn_mor        */
  NULL,                                          /* fn_not        */
  NULL,                                          /* fn_colon      */
  NULL,                                          /* fn_ctranspose */
  NULL,                                          /* fn_transpose  */
  NULL,                                          /* fn_horzcat    */
  NULL,                                          /* fn_vertcat    */
  NULL,                                          /* fn_subsref    */
  NULL,                                          /* fn_subsasgn   */
  NULL,                                          /* fn_subsindex  */

  NULL                                           /* methods */
};

