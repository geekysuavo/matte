
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_AST_H__
#define __MATTE_AST_H__

/* include the object and symbol table headers. */
#include <matte/object.h>
#include <matte/symbols.h>

/* IS_AST: macro to check that an object is a matte ast-node.
 */
#define IS_AST(obj) \
  MATTE_TYPE_CHECK(obj, ast_type())

/* AST: pointer to a struct _AST. */
typedef struct _AST *AST;
struct _ObjectType AST_type;

/* ASTNodeType: an enum _ASTNodeType. */
typedef enum _ASTNodeType ASTNodeType;

/* _ASTNodeType: enumeration that holds the types of nodes that may
 * be found within an abstract syntax tree.
 */
enum _ASTNodeType {
  AST_TYPE_NONE = 1000,

  AST_TYPE_ROOT,      /* 1001 */
  AST_TYPE_ROW,
  AST_TYPE_COLUMN,
  AST_TYPE_STATEMENTS,
  AST_TYPE_IDS,       /* 1005 */
  AST_TYPE_EMPTY,

  AST_TYPE_SYMBOL,
  AST_TYPE_CLASS,
  AST_TYPE_FUNCTION,
  AST_TYPE_FN_HANDLE, /* 1010 */
  AST_TYPE_FN_ANONY,
  AST_TYPE_FN_CALL
};

/* AST: structure for holding an abstract syntax tree.
 */
struct _AST {
  /* base object. */
  OBJECT_BASE;

  /* @up: parent syntax tree node.
   * @down: child syntax tree nodes.
   * @n_down: number of child tree nodes.
   */
  AST up, *down;
  int n_down;

  /* @node_type: type of the syntax tree node.
   * @node_data: data in the syntax tree node.
   * @node_disp: display flag of the node.
   * @has_str: if the node data is a string.
   * @pos: position in the input of the node.
   */
  ASTNodeType node_type;
  SymbolData node_data;
  bool node_disp;
  bool has_str;
  long pos;

  /* @syms: symbol table for an ast-node and its downstream sub-tree.
   * @sym_table: table of the current node's symbol.
   * @sym_index: index of the current node's symbol.
   */
  Symbols syms;
  Symbols sym_table;
  long sym_index;
};

/* function declarations (ast.c): */

ObjectType ast_type (void);

AST ast_new (Object args);

AST ast_new_with_type (ASTNodeType type);

AST ast_new_with_down (AST down);

AST ast_new_with_parms (ASTNodeType type, bool disp, AST down);

AST ast_copy (AST node);

void ast_free (AST node);

ASTNodeType ast_get_type (AST node);

long ast_get_int (AST node);

double ast_get_float (AST node);

complex double ast_get_complex (AST node);

const char *ast_get_string (AST node);

void ast_set_int (AST node, long value);

void ast_set_float (AST node, double value);

void ast_set_complex (AST node, complex double value);

void ast_set_string (AST node, const char *value);

void ast_set_type (AST node, ASTNodeType type);

void ast_set_disp (AST node, bool disp);

void ast_set_pos (AST node, long pos);

int ast_add_down (AST node, AST down);

AST ast_get_down (AST node, int index);

#define ast_first(node) ast_get_down(node, 0)

#define ast_last(node) ast_get_down(node, -1)

AST ast_get_root (AST node);

void ast_shrink_down (AST node);

AST ast_rip (AST node);

AST ast_slip (AST node, AST slip);

AST ast_merge (AST a, AST b);

Symbols ast_get_symbols (AST node);

Symbols ast_get_globals (AST node);

int ast_add_symbol (AST node, AST data, SymbolType stype);

const char *ast_get_symbol_name (AST node);

#endif /* !__MATTE_AST_H__ */

