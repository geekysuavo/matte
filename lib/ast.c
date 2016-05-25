
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the ast-node header. */
#include <matte/ast.h>

/* ast_type(): return a pointer to the ast-node object type.
 */
ObjectType ast_type (void) {
  /* return the struct address. */
  return &AST_type;
}

/* ast_new(): allocate a new matte abstract syntax tree node.
 *
 * returns:
 *  newly allocated and initialized matte ast-node.
 */
AST ast_new (Object args) {
  /* allocate a new ast-node. */
  AST node = (AST) AST_type.fn_alloc(&AST_type);
  if (!node)
    return NULL;

  /* initialize the node linkages. */
  node->up = NULL;
  node->down = NULL;
  node->n_down = 0;

  /* initialize the node type and display flag. */
  node->node_type = AST_TYPE_NONE;
  node->node_disp = false;

  /* initialize the node data. */
  node->node_data.iv = 0L;
  node->has_str = false;
  node->pos = 0L;

  /* initialize the symbol table. */
  node->syms = NULL;

  /* initialize the symbol lookup information. */
  node->sym_table = NULL;
  node->sym_index = 0L;

  /* return the new tree node. */
  return node;
}

/* ast_new_with_type(): allocate a new typed matte abstract syntax tree node.
 *
 * arguments:
 *  @type: tree node type to utilize.
 *
 * returns:
 *  newly allocated matte ast-node.
 */
AST ast_new_with_type (ASTNodeType type) {
  /* allocate a new ast-node. */
  AST node = ast_new(NULL);
  if (!node)
    return NULL;

  /* set the node type and return the new tree node. */
  ast_set_type(node, type);
  return node;
}

/* ast_new_with_down(): allocate a new matte abstract syntax tree node
 * and immediately store a downstream node into it.
 *
 * arguments:
 *  @type: tree node type to utilize.
 *  @disp: display flag to set.
 *  @down: new downstream node to append.
 *
 * returns:
 *  newly allocated matte ast-node.
 */
AST ast_new_with_down (AST down) {
  /* allocate a new ast-node. */
  AST node = ast_new(NULL);

  /* if allocation failed, free the downstream node as well. */
  if (!node) {
    object_free((Object) down);
    return NULL;
  }

  /* add the downstream node into the new node. */
  if (!ast_add_down(node, down)) {
    object_free((Object) node);
    object_free((Object) down);
    return NULL;
  }

  /* return the new tree node. */
  return node;
}

/* ast_new_with_parms(): allocate a new matte abstract syntax tree node
 * with specified node type, display flag, and downstream nodes.
 *
 * arguments:
 *  @down: new downstream node to append.
 *
 * returns:
 *  newly allocated and initialized matte ast-node.
 */
AST ast_new_with_parms (ASTNodeType type, bool disp, AST down) {
  /* allocate a new ast-node. */
  AST node = ast_new(NULL);

  /* if allocation failed, free the downstream node as well. */
  if (!node) {
    object_free((Object) down);
    return NULL;
  }

  /* set the node type and display flag. */
  ast_set_type(node, type);
  ast_set_disp(node, disp);

  /* add the downstream node into the new node. */
  if (!ast_add_down(node, down)) {
    object_free((Object) node);
    object_free((Object) down);
    return NULL;
  }

  /* return the newly allocated node. */
  return node;
}

/* ast_copy(): allocate a new matte abstract syntax tree from another
 * matte ast. this is a deep copy operation.
 *
 * arguments:
 *  @node: matte syntax tree to duplicate.
 *
 * returns:
 *  duplicated matte ast.
 */
AST ast_copy (AST node) {
  /* return null if the input argument is null. */
  if (!node)
    return NULL;

  /* allocate a new ast-node. */
  AST ndup = ast_new(NULL);
  if (!ndup)
    return NULL;

  /* set the node type and display flag. */
  ndup->up = node->up;
  ndup->node_type = node->node_type;
  ndup->node_disp = node->node_disp;
  ndup->pos = node->pos;

  /* store the node data. */
  ndup->has_str = node->has_str;
  if (node->has_str)
    ndup->node_data.sv = strdup(node->node_data.sv);
  else
    ndup->node_data = node->node_data;

  /* store the node symbol table. */
  ndup->syms = node->syms;
  ndup->sym_table = node->sym_table;
  ndup->sym_index = node->sym_index;

  /* loop over the downstream nodes. */
  for (int i = 0; i < node->n_down; i++)
    ast_add_down(ndup, ast_copy(node->down[i]));

  /* return the duplicated ast-node. */
  return ndup;
}

/* ast_free(): free all memory associated with a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to free.
 */
void ast_free (AST node) {
  /* return if the node is null. */
  if (!node)
    return;

  /* free the child nodes. */
  for (int i = 0; i < node->n_down; i++)
    object_free((Object) node->down[i]);

  /* free the array of child nodes. */
  free(node->down);

  /* free the node data based on node type. */
  if (node->has_str && node->node_data.sv)
    free(node->node_data.sv);

  /* free the symbol table. */
  if (node->syms)
    object_free((Object) node->syms);
}

/* ast_get_type(): get the type of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to access.
 *
 * returns:
 *  type of the node.
 */
ASTNodeType ast_get_type (AST node) {
  /* return empty if the node is null. */
  if (!node)
    return AST_TYPE_EMPTY;

  /* return the node type. */
  return node->node_type;
}

/* ast_get_int(): get the integer data of a matte ast-node.
 */
inline long ast_get_int (AST node) {
  /* return the node data. */
  return (node && node->has_str == false ? node->node_data.iv : 0L);
}

/* ast_get_double(): get the double data of a matte ast-node.
 */
inline double ast_get_float (AST node) {
  /* return the node data. */
  return (node && node->has_str == false ? node->node_data.fv : 0.0);
}

/* ast_get_complex(): get the complex data of a matte ast-node.
 */
inline complex double ast_get_complex (AST node) {
  /* return the node data. */
  return (node && node->has_str == false ? node->node_data.cv : 0.0);
}

/* ast_get_string(): get the string data of a matte ast-node.
 */
inline const char *ast_get_string (AST node) {
  /* return the node data. */
  return (node && node->has_str == true ? node->node_data.sv : NULL);
}

/* ast_reset_data(): reset the data field of a matte ast-node.
 *
 * arguments:
 *  @node: matte ast-node to modify. must be non-null.
 */
static inline void ast_reset_data (AST node) {
  /* free any existing string data. */
  if (node->has_str)
    free(node->node_data.sv);

  /* reset the data union and lower the string flag. */
  memset(&(node->node_data), 0, sizeof(SymbolData));
  node->has_str = false;
}

/* ast_set_int(): set the integer data of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @value: new value to store.
 */
void ast_set_int (AST node, long value) {
  /* set the integer data. */
  if (!node) return;
  ast_reset_data(node);
  node->node_data.iv = value;
}

/* ast_set_float(): set the float data of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @value: new value to store.
 */
void ast_set_float (AST node, double value) {
  /* set the float data. */
  if (!node) return;
  ast_reset_data(node);
  node->node_data.fv = value;
}

/* ast_set_complex(): set the complex data of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @value: new value to store.
 */
void ast_set_complex (AST node, complex double value) {
  /* set the complex data. */
  if (!node) return;
  ast_reset_data(node);
  node->node_data.cv = value;
}

/* ast_set_string(): set the string data of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @value: new value to store.
 */
void ast_set_string (AST node, const char *value) {
  /* set the string data. */
  if (!node) return;
  ast_reset_data(node);
  node->node_data.sv = strdup(value);
  node->has_str = true;
}

/* ast_set_type(): set the type of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @type: tree node type to utilize.
 */
void ast_set_type (AST node, ASTNodeType type) {
  /* return if the node is null. */
  if (!node)
    return;

  /* set the node type. */
  node->node_type = type;
}

/* ast_set_disp(): set the display flag of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @disp: display flag to set.
 */
void ast_set_disp (AST node, bool disp) {
  /* return if the node is null. */
  if (!node)
    return;

  /* set the node display flag. */
  node->node_disp = disp;
}

/* ast_set_pos(): set the position value of a matte ast-node.
 *
 * arguments:
 *  @node: matte ast-node to modify.
 */
inline void ast_set_pos (AST node, long pos) {
  /* set the position data. */
  if (!node) return;
  node->pos = pos;
}

/* ast_add_down(): add a downstream node to a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @down: new downstream node to append.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int ast_add_down (AST node, AST down) {
  /* validate the input arguments. */
  if (!node)
    fail("invalid input arguments");

  /* increment the downstream array size. */
  node->n_down++;

  /* reallocate the downstream array. */
  node->down = (AST*)
    realloc(node->down, node->n_down * sizeof(struct _AST**));

  /* check if reallocation failed. */
  if (!node->down)
    fail("unable to reallocate array");

  /* store the new downstream node and link it to it's parent. */
  node->down[node->n_down - 1] = down;
  if (down) down->up = node;

  /* return success. */
  return 1;
}

/* ast_get_down(): get a downstream node (by array index) from
 * a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to access.
 *  @index: array index, if non-negative. otherwise, array end offset.
 *
 * returns:
 *  downstream matte ast-node, if the index is in bounds.
 */
AST ast_get_down (AST node, int index) {
  /* validate the input arguments. */
  if (!node)
    return NULL;

  /* determine whether we've been given an offset from the beginning
   * or the end of the downstream node array.
   */
  if (index >= 0) {
    /* check bounds. */
    if (index >= node->n_down)
      return NULL;

    /* return the downstream node. */
    return node->down[index];
  }
  else {
    /* check bounds. */
    if (node->n_down + index < 0)
      return NULL;

    /* return the downstream node. */
    return node->down[node->n_down + index];
  }

  /* failure. */
  return NULL;
}

/* ast_get_root(): get the root node of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to access.
 *
 * returns:
 *  root matte ast-node.
 */
AST ast_get_root (AST node) {
  /* search up the tree to the root. */
  AST up = node;
  while (up->up)
    up = up->up;

  /* return the top node as the root, regardless of its node type. */
  return up;
}

/* ast_shrink_down(): decrease the number of downstream nodes in
 * a matte abstract syntax tree node by exactly one.
 *
 * arguments:
 *  @node: matte abstract syntax tree node to modify.
 */
inline void ast_shrink_down (AST node) {
  /* return if the node is null. */
  if (!node)
    return;

  /* free the last downfield syntax tree node, if any. */
  if (node->down && node->n_down)
    object_free((Object) node->down[node->n_down - 1]);

  /* decrement and bound the number of downstream nodes. */
  node->n_down--;
  if (node->n_down < 0)
    node->n_down = 0;
}

/* ast_rip(): rip a matte ast-node from an abstract syntax tree.
 *
 * when a node has a single downstream node, this function will
 * link over the node and free it, simplifying the tree.
 *
 * arguments:
 *  @node: matte ast-node to rip and free.
 *
 * returns:
 *  matte ast-node that has replaced the ripped node in the tree.
 */
AST ast_rip (AST node) {
  /* declare required variables:
   *  @up, @down: upstream and downstream nodes of the ripped node.
   *  @i: index of the ripped node in the upstream's node array.
   */
  AST up, down;
  int i;

  /* do not rip null nodes or nodes without a single child. */
  if (!node || node->n_down != 1)
    return NULL;

  /* do not rip nodes without a parent. */
  up = node->up;
  if (!up)
    return NULL;

  /* store the downstream node. */
  down = node->down[0];
  node->down[0] = NULL;

  /* lookup the index of the ripped node in the upstream's array. */
  for (i = 0; i < up->n_down; i++) {
    if (up->down[i] == node)
      break;
  }

  /* link over the ripped node. */
  down->up = up;
  up->down[i] = down;

  /* free the ripped node and return its replacement. */
  object_free((Object) node);
  return down;
}

/* ast_slip(): slip a matte ast-node into an abstract syntax tree,
 * essentially the reverse operation of ast_rip().
 *
 * arguments:
 *  @node: matte ast-node to slip a node before.
 *
 * returns:
 *  matte ast-node that has replaced the node in the tree.
 */
AST ast_slip (AST node, AST slip) {
  /* declare required variables:
   *  @up: upstream node of the slipped node.
   *  @i: index of the insertion node in the upstream's node array.
   */
  AST up;
  int i;

  /* do not slip null nodes into the tree. */
  if (!node || !slip)
    return NULL;

  /* do not slip before nodes without a parent. */
  up = node->up;
  if (!up)
    return NULL;

  /* lookup the index of the insertion node in the upstream's array. */
  for (i = 0; i < up->n_down; i++) {
    if (up->down[i] == node)
      break;
  }

  /* insert the new node into the tree. */
  ast_add_down(slip, node);
  up->down[i] = slip;
  slip->up = up;

  /* return the inserted node. */
  return slip;
}

/* ast_merge(): merge two abstract syntax trees into one.
 *
 * arguments:
 *  @a, @b: first and second syntax tree of the merge.
 *
 * returns:
 *  merged result of the two trees.
 */
AST ast_merge (AST a, AST b) {
  /* declare required variables:
   *  @i: general-purpose loop counter.
   */
  int i;

  /* if neither tree is allocated, return an empty root. */
  if (!a && !b)
    return ast_new_with_type(AST_TYPE_ROOT);

  /* if the first tree is unallocated, root and return the second. */
  if (!a && b) {
    if (b->node_type == AST_TYPE_ROOT)
      return b;
    else
      return ast_new_with_parms(AST_TYPE_ROOT, false, b);
  }

  /* if the second tree is unallocated, root and return the first. */
  if (!b && a) {
    if (a->node_type == AST_TYPE_ROOT)
      return a;
    else
      return ast_new_with_parms(AST_TYPE_ROOT, false, a);
  }

  /* both trees are allocated: determine how to merge them. */
  if (a->node_type == AST_TYPE_ROOT && b->node_type == AST_TYPE_ROOT) {
    /* add each block of the second tree as a block of the first. */
    for (i = 0; i < b->n_down; i++) {
      ast_add_down(a, b->down[i]);
      b->down[i] = NULL;
    }

    /* free the extra root and return the combined tree. */
    object_free((Object) b);
    return a;
  }
  else if (a->node_type == AST_TYPE_ROOT) {
    /* add the second tree as a block of the first. */
    ast_add_down(a, b);
    return a;
  }
  else if (b->node_type == AST_TYPE_ROOT) {
    /* create a new rooted tree with the first tree as a child. */
    AST super = ast_new_with_parms(AST_TYPE_ROOT, false, a);

    /* add each block of the second tree as a block of the first. */
    for (i = 0; i < b->n_down; i++) {
      ast_add_down(super, b->down[i]);
      b->down[i] = NULL;
    }

    /* free the extra root and return the combined tree. */
    object_free((Object) b);
    return super;
  }

  /* neither tree is rooted with a blocks node. */
  AST super = ast_new_with_type(AST_TYPE_ROOT);
  ast_add_down(super, a);
  ast_add_down(super, b);
  return super;
}

/* ast_get_symbols(): get the symbol table that holds symbols for the
 * current matte ast-node or sub-tree.
 *
 * arguments:
 *  @node: matte syntax tree node to access.
 *
 * returns:
 *  symbol table for the matte ast-node, if available.
 */
Symbols ast_get_symbols (AST node) {
  /* search up the tree for the symbol table. */
  AST up = node;
  while (up) {
    /* if the current node has a table, return it. */
    if (up->syms)
      return up->syms;

    /* move up the tree. */
    up = up->up;
  }

  /* no symbol table available. */
  return NULL;
}

/* ast_get_globals(): get the global symbol table for the current
 * matte ast-node or sub-tree.
 *
 * arguments:
 *  @node: matte syntax tree node to access.
 *
 * returns:
 *  global symbol table for the node.
 */
Symbols ast_get_globals (AST node) {
  /* return the symbol table of the root node. */
  AST root = ast_get_root(node);
  return root->syms;
}

/* symbols_add_from_ast(): helper function for ast_add_symbol() that
 * registers a symbol with a symbol table using ast-node data, but
 * does not store the resulting symbol lookup information in any
 * final place.
 *
 * arguments:
 *  @syms: matte symbol table to modify.
 *  @stype: symbol type to register.
 *  @data: matte ast-node to use for data.
 *
 * returns:
 *  symbol index (or status) returned from symbols_add().
 */
static inline long
symbols_add_from_ast (Symbols syms, SymbolType stype, AST data) {
  /* register the symbol data based on symbol type. */
  if (stype & SYMBOL_LITERAL) {
    /* supply the literal data. */
    return symbols_add(syms, stype, NULL, data->node_data);
  }
  else {
    /* supply no data. */
    return symbols_add(syms, stype, data->node_data.sv);
  }

  /* return failure. */
  return 0;
}

/* ast_add_symbol(): add a symbol into a matte abstract syntax tree.
 *
 * arguments:
 *  @node: matte ast-node to register the symbol with.
 *  @data: matte ast-node to access for data.
 *  @stype: symbol type to register.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int ast_add_symbol (AST node, AST data, SymbolType stype) {
  /* declare required variables:
   *  @super: ast node for tree searching.
   *  @gs, @ls: destination symbol tables.
   *  @gid, @lid: symbol indices from the tables.
   */
  Symbols gs, ls;
  long gid, lid;
  AST super;

  /* initialize the variables. */
  gs = ls = NULL;

  /* check if a global symbol registration was requested. */
  if (stype & SYMBOL_GLOBAL)
    gs = ast_get_globals(node);

  /* loop until a suitable local symbol table is found. */
  super = node;
  while (super) {
    /* if a symbol table is available, store into it. */
    if (super->syms) {
      ls = super->syms;
      break;
    }

    /* move to the upstream node. */
    super = super->up;
  }

  /* fail if no local symbol table was located. */
  if (!ls)
    return 0;

  /* store the registered symbol index. checks within symbols_add()
   * will ensure that symbol duplication will not occur if gs==ls.
   */
  lid = symbols_add_from_ast(ls, stype, data);
  gid = symbols_add_from_ast(gs, stype, data);

  /* check that symbol registration was successful. */
  if (gs && gid) {
    /* store the global symbol information. */
    node->sym_index = gid;
    node->sym_table = gs;
  }
  else if (ls && lid) {
    /* store the local symbol information. */
    node->sym_index = lid;
    node->sym_table = ls;
  }
  else
    return 0;

  /* return success. */
  return 1;
}

/* ast_get_symbol_name(): get the name of the symbol registered with
 * a matte ast-node.
 *
 * arguments:
 *  @node: matte ast-node to access.
 *
 * returns:
 *  name string of the current node.
 */
const char *ast_get_symbol_name (AST node) {
  /* if a symbol is registered with the node, then return it's name. */
  if (node->sym_index && node->sym_table)
    return symbol_name(node->sym_table, node->sym_index - 1);

  /* otherwise, return null. */
  return NULL;
}

/* AST_type: object type structure for matte abstract syntax tree nodes.
 */
struct _ObjectType AST_type = {
  "AST",                               /* name       */
  sizeof(struct _AST),                 /* size       */
  0,                                   /* precedence */

  (obj_constructor) ast_new,           /* fn_new     */
  (obj_allocator)   object_alloc,      /* fn_alloc   */
  (obj_destructor)  ast_free,          /* fn_dealloc */
  NULL,                                /* fn_disp    */

  NULL,                                /* fn_plus       */
  NULL,                                /* fn_minus      */
  NULL,                                /* fn_uminus     */
  NULL,                                /* fn_times      */
  NULL,                                /* fn_mtimes     */
  NULL,                                /* fn_rdivide    */
  NULL,                                /* fn_ldivide    */
  NULL,                                /* fn_mrdivide   */
  NULL,                                /* fn_mldivide   */
  NULL,                                /* fn_power      */
  NULL,                                /* fn_mpower     */
  NULL,                                /* fn_lt         */
  NULL,                                /* fn_gt         */
  NULL,                                /* fn_le         */
  NULL,                                /* fn_ge         */
  NULL,                                /* fn_ne         */
  NULL,                                /* fn_eq         */
  NULL,                                /* fn_and        */
  NULL,                                /* fn_or         */
  NULL,                                /* fn_mand       */
  NULL,                                /* fn_mor        */
  NULL,                                /* fn_not        */
  NULL,                                /* fn_colon      */
  NULL,                                /* fn_ctranspose */
  NULL,                                /* fn_transpose  */
  NULL,                                /* fn_horzcat    */
  NULL,                                /* fn_vertcat    */
  NULL,                                /* fn_subsref    */
  NULL,                                /* fn_subsasgn   */
  NULL                                 /* fn_subsindex  */
};

