
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
  node->data_str = NULL;
  node->data_int = 0;
  node->data_float = 0.0;
  node->data_complex = 0.0;

  /* initialize the symbol table. */
  node->syms = NULL;

  /* initialize the symbol lookup information. */
  node->sym_table = NULL;
  node->sym_index = 0;

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

/* ast_new_with_disp(): allocate a new matte abstract syntax tree node
 * with a specified display flag value.
 *
 * arguments:
 *  @disp: display flag to set.
 *
 * returns:
 *  newly allocated matte ast-node.
 */
AST ast_new_with_disp (bool disp) {
  /* allocate a new ast-node. */
  AST node = ast_new(NULL);
  if (!node)
    return NULL;

  /* set the display flag and return the new tree node. */
  ast_set_disp(node, disp);
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

  /* store the node string data. */
  if (node->data_str) ndup->data_str = strdup(node->data_str);

  /* store the node numeric data. */
  ndup->data_int     = node->data_int;
  ndup->data_float   = node->data_float;
  ndup->data_complex = node->data_complex;

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

  /* free the node data. */
  if (node->data_str)
    free(node->data_str);

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

/* ast_set_up(): set the upstream node of a matte ast-node.
 *
 * arguments:
 *  @node: matte syntax tree node to modify.
 *  @up new upstream node to set.
 */
void ast_set_up (AST node, AST up) {
  /* return if either node is null. */
  if (!node || !up)
    return;

  /* set the upstream node. */
  node->up = up;
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

/* ast_find_symbol(): find a symbol in a matte abstract syntax tree.
 *
 * arguments:
 *  @node: matte ast-node to access.
 *  @stype: symbol type to lookup.
 *  @sname: symbol name to lookup.
 *
 * returns:
 *  integer indicating lookup success (1) or failure (0).
 */
int ast_find_symbol (AST node, SymbolType stype, const char *sname) {
  /* loop until a symbol table is found. */
  AST super = node;
  while (super) {
    /* if a symbol table is available, lookup the symbol. */
    if (symbols_find(super->syms, stype, sname))
      return 1;

    /* no symbol found. move to the upstream node. */
    super = super->up;
  }

  /* return 'not found' */
  return 0;
}

/* ast_add_symbol(): add a symbol into a matte abstract syntax tree.
 *
 * arguments:
 *  @node: matte ast-node to modify.
 *  @stype: symbol type to register.
 *  @sname: symbol name to register.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int ast_add_symbol (AST node, SymbolType stype, const char *sname) {
  /* loop until a symbol table is found. */
  AST super = node;
  while (super) {
    /* if a symbol table is available, register the symbol. */
    if (super->syms) {
      /* store and return the symbol index. */
      node->sym_index = symbols_add(super->syms, stype, sname);

      /* check that symbol addition was successful. */
      if (node->sym_index)
        node->sym_table = super->syms;
      else
        return 0;

      /* return success. */
      return 1;
    }

    /* move to the upstream node. */
    super = super->up;
  }

  /* return failure. */
  return 0;
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
    return node->sym_table->sym_name[node->sym_index - 1];

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

