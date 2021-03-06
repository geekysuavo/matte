
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the compiler and builtins headers. */
#include <matte/compiler.h>
#include <matte/builtins.h>
#include <matte/except.h>

/* W(): macro function for writing to the c source code string
 * of a matte compiler.
 */
#define W(...) string_appendf(c->ccode, __VA_ARGS__)

/* S(): macro function for obtaining the symbol name of a matte ast-node.
 */
#define S(nd) ast_get_symbol_name(nd)

/* E(): macro function for writing an exception handler using data
 * from a matte ast-node.
 */
#define E(var, nd) \
  if (c->catching) { \
    W("  EXCEPT_CATCH(%s, %s, %s, \"%s\", \"%s\", %ld);\n", \
      var, c->cvar, c->clbl, nd->fname, ast_get_func(nd), nd->line); \
  } else { \
    W("  EXCEPT_HANDLE(%s, \"%s\", \"%s\", %ld);\n", \
      var, nd->fname, ast_get_func(nd), nd->line); }

/* NEW_LABEL: macro function to construct a new label string and store it
 * in the current matte compiler.
 */
#define NEW_LABEL \
  sprintf(c->clbl, "lbl%ld", c->cidx++)

/* asterr(): macro function to print an immediate compiler error message
 * using information from a matte ast-node.
 */
#define asterr(nd, ...) \
  { exceptions_add(nd->fname, ast_get_func(nd), nd->line, __VA_ARGS__); \
    return 0; }

/* operators: array of c function names that are mapped to overloadable
 * operations in the matte compiler.
 */
static const struct {
  /* @tok: node type (token) of the matte ast-node containing the operation.
   * @noper: number of downfield operands in the operation.
   * @fstr: function name of the overloadable operation.
   */
  ScannerToken tok;
  const int noper;
  const char *fstr;
}
operators[] = {
  { T_PLUS,          2, "object_plus"       },
  { T_MINUS,         2, "object_minus"      },
  { T_MINUS,         1, "object_uminus"     },
  { T_ELEM_MUL,      2, "object_times"      },
  { T_MUL,           2, "object_mtimes"     },
  { T_ELEM_DIV,      2, "object_rdivide"    },
  { T_ELEM_LDIV,     2, "object_ldivide"    },
  { T_DIV,           2, "object_mrdivide"   },
  { T_LDIV,          2, "object_mldivide"   },
  { T_ELEM_POW,      2, "object_power"      },
  { T_POW,           2, "object_mpower"     },
  { T_LT,            2, "object_lt"         },
  { T_GT,            2, "object_gt"         },
  { T_LE,            2, "object_le"         },
  { T_GE,            2, "object_ge"         },
  { T_NE,            2, "object_ne"         },
  { T_EQ,            2, "object_eq"         },
  { T_ELEM_AND,      2, "object_and"        },
  { T_ELEM_OR,       2, "object_or"         },
  { T_AND,           2, "object_mand"       },
  { T_OR,            2, "object_mor"        },
  { T_NOT,           1, "object_not"        },
  { T_COLON,         3, "object_colon"      },
  { T_HTR,           1, "object_ctranspose" },
  { T_TR,            1, "object_transpose"  },
  { T_ERR,           0, NULL                }
};

/* simplify_concats(): simplify concatenation operations by compressing
 * trivial operation groups.
 *
 * arguments:
 *  @node: matte ast-node to process.
 */
static void simplify_concats (AST node) {
  /* declare required variables:
   *  @down: downstream nodes returned from ripping.
   *  @i: general-purpose loop counter.
   */
  AST down;
  int i;

  /* do not traverse null nodes. */
  if (!node) return;

  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);

  /* check if the node is a column or row. */
  if (ntype == AST_TYPE_COLUMN) {
    /* compress col->(row) into row. */
    if (node->n_down == 1 && ast_get_type(node->down[0]) == AST_TYPE_ROW) {
      simplify_concats(ast_rip(node));
      return;
    }
  }
  else if (ntype == AST_TYPE_ROW && node->n_down == 1 &&
           ast_get_type(node->down[0]) != (ASTNodeType) T_COLON) {
    /* compress row->(expr) into expr for all expr != range. */
    simplify_concats(ast_rip(node));
    return;
  }

  /* traverse further into the tree. */
  for (i = 0; i < node->n_down; i++)
    simplify_concats(node->down[i]);
}

/* init_symbols(): initialize all symbols defined within a compiler's
 * abstract syntax tree.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int init_symbols (Compiler c, AST node) {
  /* declare required variables:
   *  @down: general-purpose downstream node.
   *  @sid: symbol index variable.
   *  @i: general-purpose loop counter.
   */
  AST down;
  long sid;
  int i;

  /* do not traverse null nodes. */
  if (!node) return 1;

  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);
  const ScannerToken ntok = (ScannerToken) ntype;

  /* get the current variable symbol type based on symbol scope. */
  SymbolType vartype = SYMBOL_VAR;
  if (ast_get_symbols(node) == ast_get_globals(node))
    vartype |= SYMBOL_GLOBAL;

  /* allocate symbol tables for certain node types. */
  if (ntype == AST_TYPE_ROOT ||
      ntype == AST_TYPE_CLASS ||
      ntype == AST_TYPE_FUNCTION) {
    /* allocate a symbol table. */
    node->syms = symbols_new(NULL, NULL);
    if (!node->syms)
      return 0;
  }

  /* check if the current node is the root. */
  if (ntype == AST_TYPE_ROOT) {
    /* pre-register global built-ins with the global table. */
    if (!matte_builtins_init(node->syms))
      return 0;
  }

  /* traverse based on node type. */
  if (ntype == AST_TYPE_CLASS) {
    /* register the new class name. */
    down = node->down[0];
    if (!ast_add_symbol(down, down, SYMBOL_GLOBAL_CLASS))
      return 0;
  }
  else if (ntype == AST_TYPE_FUNCTION) {
    /* register the input arguments (third child). */
    if (node->down[2]) {
      /* register a symbol with each node. */
      for (i = 0; i < node->down[2]->n_down; i++) {
        down = node->down[2]->down[i];
        if (!ast_add_symbol(down, down, SYMBOL_ARGIN))
          return 0;
      }
    }

    /* register the output arguments (first child). */
    if (node->down[0]) {
      /* determine the argument count. */
      if (node->down[0]->n_down) {
        /* multiple arguments. */
        for (i = 0; i < node->down[0]->n_down; i++) {
          down = node->down[0]->down[i];
          if (!ast_add_symbol(down, down, SYMBOL_ARGOUT))
            return 0;
        }
      }
      else {
        /* single argument. */
        down = node->down[0];
        if (!ast_add_symbol(down, down, SYMBOL_ARGOUT))
          return 0;
      }
    }

    /* register the function name (second child). */
    if (!ast_add_symbol(node->down[1], node->down[1], SYMBOL_GLOBAL_FUNC))
      return 0;

    /* traverse the function statements (fourth child). */
    if (!init_symbols(c, node->down[3]))
      return 0;
  }
  else if (ntok == T_FOR) {
    /* register the iteration variable (first child). */
    if (!ast_add_symbol(node->down[0], node->down[0], SYMBOL_VAR))
      return 0;

    /* register a symbol for the iterator. */
    if (!symbols_add(ast_get_symbols(node), SYMBOL_VAR, "_it"))
      return 0;

    /* traverse the loop expression and statements (second, third child). */
    if (!init_symbols(c, node->down[1]) ||
        !init_symbols(c, node->down[2]))
      return 0;
  }
  else if (ntok == T_SWITCH) {
    /* register a symbol for the comparison result. */
    if (!symbols_add(ast_get_symbols(node), SYMBOL_VAR, "_sw"))
      return 0;

    /* traverse the rest of the block. */
    for (i = 0; i < node->n_down; i++) {
      if (!init_symbols(c, node->down[i]))
        return 0;
    }
  }
  else if (ntok == T_TRY) {
    /* register the catch variable. */
    if (!ast_add_symbol(node->down[1], node->down[1], SYMBOL_VAR))
      return 0;

    /* traverse the try and catch statement lists (first, third child). */
    if (!init_symbols(c, node->down[0]) ||
        !init_symbols(c, node->down[2]))
      return 0;
  }
  else if (ntok == T_GLOBAL) {
    /* register global symbols. */
    for (i = 0; i < node->n_down; i++) {
      if (!ast_add_symbol(node, node->down[i], SYMBOL_GLOBAL_VAR))
        return 0;
    }
  }
  else if (ntok == T_PERSISTENT) {
    /* register persistent symbols. */
    for (i = 0; i < node->n_down; i++) {
      if (!ast_add_symbol(node, node->down[i], SYMBOL_STATIC_VAR))
        return 0;
    }
  }
  else if (ntok == T_ASSIGN) {
    /* check the type of the left-hand side. */
    if (ast_get_type(node->down[0]) == AST_TYPE_ROW) {
      /* register names of the matrix lhs. */
      AST row = node->down[0];
      for (i = 0; i < row->n_down; i++) {
        if (!ast_add_symbol(node, row->down[i], vartype))
          return 0;
      }
    }
    else {
      /* register the name lhs. */
      if (!ast_add_symbol(node, node->down[0], vartype))
        return 0;
    }

    /* register symbols in the right-hand side expressions. */
    if (!init_symbols(c, node->down[1]))
      return 0;
  }
  else if (ntok == T_INT) {
    /* register the integer literal. */
    if (!ast_add_symbol(node, node, SYMBOL_INT))
      return 0;
  }
  else if (ntok == T_FLOAT) {
    /* register the float literal. */
    if (!ast_add_symbol(node, node, SYMBOL_FLOAT))
      return 0;
  }
  else if (ntok == T_COMPLEX) {
    /* register the complex literal. */
    if (!ast_add_symbol(node, node, SYMBOL_COMPLEX))
      return 0;
  }
  else if (ntok == T_STRING) {
    /* register the string literal. */
    if (!ast_add_symbol(node, node, SYMBOL_STRING))
      return 0;
  }
  else {
    /* do not register empty or end symbols. */
    if (ntype == AST_TYPE_EMPTY || ntok == T_END)
      return 1;
    
    /* register intermediate symbols in expressions. */
    if (ntype == AST_TYPE_ROW || ntype == AST_TYPE_COLUMN ||
        (ntok >= T_INC && ntok <= T_OR)) {
      /* register the symbol. */
      if (!ast_add_symbol(node, node, SYMBOL_TEMP_VAR))
        return 0;
    }

    /* perform general traversal. */
    for (i = 0; i < node->n_down; i++) {
      if (!init_symbols(c, node->down[i]))
        return 0;
    }
  }

  /* return success. */
  return 1;
}

/* resolve_symbols(): resolve all symbols that are accessed by
 * operations within an abstract syntax tree.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int resolve_symbols (Compiler c, AST node) {
  /* declare required variables:
   *  @super: upstream ast-node for symbol searching.
   *  @syms: symbol table for symbol searching.
   *  @sid: symbol index from table lookups.
   *  @i: general-purpose loop counter.
   */
  AST super;
  Symbols syms;
  long sid;
  int i;

  /* do not traverse null nodes. */
  if (!node) return 1;

  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);
  const ScannerToken ntok = (ScannerToken) ntype;

  /* check if the node is an identifier. */
  if (ntok == T_IDENT) {
    /* search up the tree for the symbol. */
    super = node;
    while (super) {
      /* lookup the symbol in the current node's table. */
      sid = symbols_find(super->syms, SYMBOL_ANY, ast_get_string(node));
      if (sid) {
        /* symbol found. store it in the node. */
        node->sym_table = super->syms;
        node->sym_index = sid;

        /* check if the symbol is a function. */
        if (symbol_has_type(super->syms, sid - 1, SYMBOL_FUNC)) {
          /* get the symbol table for the node. */
          syms = ast_get_symbols(node);
          if (!syms) return 0;

          /* register symbols for the function arguments and results. */
          if (!symbols_add(syms, SYMBOL_VAR, "_ai") ||
              !symbols_add(syms, SYMBOL_VAR, "_ao"))
            return 0;

          /* determine the call mode of the function. */
          if (ast_get_type(node->up) == (ASTNodeType) T_ASSIGN) {
            /* handle the assignment using a function call. */
            node->up->node_type = AST_TYPE_FN_CALL;
          }
          else {
            /* insert an assignment for the function call. */
            super = ast_new_with_type((ASTNodeType) T_IDENT);
            super = ast_new_with_parms(AST_TYPE_FN_CALL, false, super);
            if (!ast_slip(node, super))
              return 0;

            /* register a temporary symbol for the result. */
            if (!ast_add_symbol(super->down[0], super->down[0],
                                SYMBOL_TEMP_VAR))
              return 0;

            /* also store the temporary symbol at the root node
             * of the function call.
             */
            super->sym_table = super->down[0]->sym_table;
            super->sym_index = super->down[0]->sym_index;
          }
        }

        /* discontinue the search. */
        break;
      }

      /* traverse up the tree. */
      super = super->up;
    }

    /* FIXME: if still unresolved, search the path. */

    /* check if no symbol was found. */
    if (!node->sym_table || !node->sym_index)
      asterr(node, ERR_UNDEFINED_SYMBOL, ast_get_string(node));
  }
  else if (ntype == AST_TYPE_FUNCTION) {
    /* traverse only into the statement list. */
    if (!resolve_symbols(c, node->down[3]))
      return 0;

    /* return success. */
    return 1;
  }

  /* traverse further into the tree. */
  for (i = 0; i < node->n_down; i++) {
    if (!resolve_symbols(c, node->down[i]))
      return 0;
  }

  /* return success. */
  return 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* forward declarations: */
static void write_statements (Compiler c, AST node);

/* write_operation(): write a single operation, or nothing if the specified
 * ast-node is not a supported operation.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *
 * returns:
 *  integer indicating whether the write was performed.
 */
static int write_operation (Compiler c, AST node) {
  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);

  /* search the operator definition array for the current node type. */
  for (int i = 0; operators[i].fstr; i++) {
    /* check if a match was found. */
    if (ntype == (ASTNodeType) operators[i].tok &&
        node->n_down == operators[i].noper) {
      /* write the operation based on argument count. */
      if (node->n_down == 1) {
        /* write a unary operation. */
        W("  Object %s = %s(&_z1, %s);\n",
          S(node), operators[i].fstr,
          S(node->down[0]));
      }
      else if (node->n_down == 2) {
        /* write a binary operation. */
        W("  Object %s = %s(&_z1, %s, %s);\n",
          S(node), operators[i].fstr,
          S(node->down[0]),
          S(node->down[1]));
      }
      else if (node->n_down == 3) {
        /* write a ternary operation. */
        W("  Object %s = %s(&_z1, %s, %s, %s);\n",
          S(node), operators[i].fstr,
          S(node->down[0]),
          S(node->down[1]),
          S(node->down[2]));
      }

      /* write the error handler and return valid. */
      E(S(node), node);
      return 1;
    }
  }

  /* not a valid operation. */
  return 0;
}

/* write_concat(): write a single concatenation, or nothing if the
 * specified ast-node is not a horzcat() or vertcat().
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *
 * returns:
 *  integer indicating whether the write was performed.
 */
static int write_concat (Compiler c, AST node) {
  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);

  /* accept rows and columns. */
  if (ntype == AST_TYPE_ROW) {
    /* write the row (horizontal) concatenation. */
    W("  Object %s = object_horzcat(&_z1, %d", S(node), node->n_down);
    for (int i = 0; i < node->n_down; i++)
      W(", %s", S(node->down[i]));
    W(");\n");

    /* write the error handler and return valid. */
    E(S(node), node);
    return 1;
  }
  else if (ntype == AST_TYPE_COLUMN) {
    /* write the column (vertical) concatenation. */
    W("  Object %s = object_vertcat(&_z1, %d", S(node), node->n_down);
    for (int i = 0; i < node->n_down; i++)
      W(", %s", S(node->down[i]));
    W(");\n");

    /* write the error handler and return valid. */
    E(S(node), node);
    return 1;
  }

  /* not a concatenation. */
  return 0;
}

/* write_assign(): write a single assignment, or nothing if the
 * specified ast-node is not an assignment statement.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *
 * returns:
 *  integer indicating whether the write was performed.
 */
static int write_assign (Compiler c, AST node) {
  /* get the current node type. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);

  /* accept only assignment nodes. */
  if (ntok != T_ASSIGN)
    return 0;

  /* assign based on scope. */
  if (ast_has_global_symbol(node))
    W("  %s = object_copy(&_zg, %s);\n", S(node), S(node->down[1]));
  else
    W("  %s = %s;\n", S(node), S(node->down[1]));

  /* return true. */
  return 1;
}

/* write_call(): write a single function or method call, or nothing if the
 * specified ast-node is not a call.
 */
static int write_call (Compiler c, AST node) {
  /* declare required variables:
   *  @sname: symbol name string storage variable.
   *  @down: general-purpose child ast-node.
   *  @i: general-purpose loop counter.
   */
  const char *sname;
  AST down;
  int i;

  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);

  /* reject non-calls. */
  if (ntype != AST_TYPE_FN_CALL &&
      ntype != AST_TYPE_MD_CALL &&
      ntype != AST_TYPE_CTOR)
    return 0;

  /* write based on node type. */
  if (ntype == AST_TYPE_FN_CALL) {
    /* FIXME: comment and refactor. */
    down = node->down[1];
    if (down->n_down == 1 &&
        ast_get_type(down->down[0]) == (ASTNodeType) T_PAREN_OPEN) {
      down = down->down[0];
      W("  _ai = object_list_argin(&_z1, %d", down->n_down);
      for (i = 0; i < down->n_down; i++)
        W(", %s", S(down->down[i]));
      W(");\n");
    }
    else {
      W("  _ai = object_list_argin(&_z1, 0);\n");
    }

    down = node->down[1];
    W("  _ao = matte_%s(&_z1, _ai);\n", S(down));
    E("_ao", down);

    down = node->down[0];
    if (ast_get_type(down) == (ASTNodeType) T_IDENT) {
      sname = S(down);
      if (ast_has_global_symbol(down))
        W("  %s = object_copy(&_zg, "
          "object_list_get((ObjectList) _ao, 0));\n", sname);
      else
        W("  %s%s = object_list_get((ObjectList) _ao, 0);\n",
          sname[0] == '_' ? "Object " : "", sname);
    }
    else if (ast_get_type(down) == AST_TYPE_ROW) {
      for (i = 0; i < down->n_down; i++) {
        if (ast_has_global_symbol(down->down[i]))
          W("  %s = object_copy(&_zg, "
            "object_list_get((ObjectList) _ao, %d));\n",
            S(down->down[i]), i);
        else
          W("  %s = object_list_get((ObjectList) _ao, %d);\n",
            S(down->down[i]), i);
      }
    }
  }

  /* write code to free the argument lists. */
  W("  object_free(&_z1, _ai);\n");
  W("  object_free(&_z1, _ao);\n");

  /* return true. */
  return 1;
}

/* write_try(): write a try/catch-statement block, or nothing if the
 * specified ast-node is not a try/catch block.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 */
static int write_try (Compiler c, AST node) {
  /* accept try-statement blocks. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);
  if (ntok != T_TRY)
    return 0;

  /* feebly attempt to disallow nesting of try within try. */
  AST up = node->up;
  while (up) {
    /* fail on encountering a try. */
    if (ast_get_type(up) == (ASTNodeType) T_TRY &&
        ast_contains(up->down[0], node))
      asterr(node, ERR_INVALID_TRY);

    /* move up the tree. */
    up = up->up;
  }

  /* store the name of the catch variable. */
  c->cvar = S(node->down[1]);

  /* catch any exceptions from the try block. */
  c->catching = true;
  write_statements(c, node->down[0]);
  c->catching = false;

  /* store a copy of the catch block label, and generate a new label. */
  char *lbl = strdup(c->clbl);
  NEW_LABEL;

  /* write code that will bypass the catch if no exceptions occurred. */
  W("  goto %s;\n%s: ;\n", c->clbl, lbl);

  /* store the new label string. */
  free(lbl);
  lbl = strdup(c->clbl);
  NEW_LABEL;

  /* write the catch statements. */
  write_statements(c, node->down[2]);
  W("%s: ;\n", lbl);
  free(lbl);

  /* unset the catch variable name. */
  c->cvar = NULL;

  /* return true. */
  return 1;
}

/* write_if(): write an if-statement block, or nothing if the specified
 * ast-node is not an if block.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *  @i: offset in the child node array.
 */
static int write_if (Compiler c, AST node, int i) {
  /* accept if-statement blocks. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);
  if (ntok != T_IF)
    return 0;

  /* return if we've exhausted the condition list. */
  if (i >= node->n_down)
    return 1;

  /* get references to the child nodes. */
  AST expr = node->down[i];
  AST stmts = node->down[i + 1];

  /* check what type of node we're processing. */
  if (expr) {
    /* write the current condition evaluation. */
    write_statements(c, expr);
    W("  if (object_true(%s)) {\n", S(expr));
    write_statements(c, stmts);
    W("  }\n");

    /* return if no further conditions exist. */
    if (node->n_down == i + 2)
      return 1;

    /* write the next condition evaluations. */
    W("  else {\n");
    write_if(c, node, i + 2);
    W("  }\n");
  }
  else {
    /* write the 'else' statements. */
    write_statements(c, stmts);
  }

  /* return true. */
  return 1;
}

/* write_switch(): write a switch block, or nothing if the specified
 * ast-node is not a switch block.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *  @i: offset in the child node array.
 */
static int write_switch (Compiler c, AST node, int i) {
  /* accept switch blocks. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);
  if (ntok != T_SWITCH)
    return 0;

  /* return if we've exhausted the condition list. */
  if (i >= node->n_down)
    return 1;

  /* get references to the child nodes. */
  AST expr = node->down[0];
  AST value = node->down[i];
  AST stmts = node->down[i + 1];

  /* upon entering the switch, evaluate its expression. */
  if (i == 0) {
    write_statements(c, expr);
    return write_switch(c, node, 1);
  }

  /* check what type of node we're processing. */
  if (value) {
    /* write the current condition evaluation. */
    write_statements(c, value);
    W("  _sw = object_eq(&_z1, %s, %s);\n", S(expr), S(value));
    E("_sw", value);
    W("  if (object_true(_sw)) {\n"
      "  object_free(&_z1, _sw);\n");
    write_statements(c, stmts);
    W("  }\n");

    /* write the following cases. */
    W("  else {\n"
      "  object_free(&_z1, _sw);\n");
    write_switch(c, node, i + 2);
    W("  }\n");
  }
  else {
    /* write the 'otherwise' statements. */
    write_statements(c, stmts);
  }

  /* return true. */
  return 1;
}

/* write_for(): write a for-statement block, or nothing if the specified
 * ast-node is not a for loop.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *  @i: offset in the child node array.
 */
static int write_for (Compiler c, AST node) {
  /* accept for loops. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);
  if (ntok != T_FOR)
    return 0;

  /* determine the scope/zone of the iteration variable. */
  const char *itzone = (ast_has_global_symbol(node->down[0]) ?
                        "&_zg" : "&_z1");

  /* get references to the child nodes. */
  AST var = node->down[0];
  AST expr = node->down[1];
  AST stmts = node->down[2];

  /* evaluate the iteration expression. */
  write_statements(c, expr);

  /* create an iterator from the evaluated expression. */
  W("  _it = (Object) iter_new(&_z1, %s);\n", S(expr));
  E("_it", var);

  /* write the loop head and variable assignment. */
  W("  while (iter_next(%s, (Iter) _it)) {\n", itzone);
  W("  %s = iter_get_value((Iter) _it);\n", S(var));
  E(S(var), var);

  /* write the body of the loop. */
  write_statements(c, stmts);
  W("  }\n");

  /* write code to free the iterator. */
  W("  object_free(&_z1, _it);\n");

  /* return true. */
  return 1;
}

/* write_while(): write a while loop block, or nothing if the specified
 * ast-node is not a while loop.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *  @i: offset in the child node array.
 */
static int write_while (Compiler c, AST node) {
  /* accept while loops. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);
  if (ntok != T_WHILE)
    return 0;

  /* get references to the child nodes. */
  AST expr = node->down[0];
  AST stmts = node->down[1];

  /* write the while block. */
  W("  while (1) {\n");
  write_statements(c, expr);
  W("  if (!object_true(%s)) break;\n", S(expr));
  write_statements(c, stmts);
  W("  }\n");

  /* return true. */
  return 1;
}

/* write_until(): write a do-until loop block, or nothing if the specified
 * ast-node is not a do-until loop.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *  @i: offset in the child node array.
 */
static int write_until (Compiler c, AST node) {
  /* accept do-until loops. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);
  if (ntok != T_UNTIL)
    return 0;

  /* get references to the child nodes. */
  AST stmts = node->down[0];
  AST expr = node->down[1];

  /* write the do-until block. */
  W("  while (1) {\n");
  write_statements(c, stmts);
  write_statements(c, expr);
  W("  if (object_true(%s)) break;\n", S(expr));
  W("  }\n");

  /* return true. */
  return 1;
}

/* write_flow(): write a control flow statement, or nothing if the specified
 * ast-node is not a such a statement.
 *
 * arguments;
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 */
static int write_flow (Compiler c, AST node) {
  /* get the current node type. */
  const ScannerToken ntok = (ScannerToken) ast_get_type(node);

  /* for break and continue, check that the node is within a loop. */
  AST loop = node;
  while (loop) {
    /* break upon encountering a loop node. */
    if (ast_get_type(loop) == (ASTNodeType) T_FOR ||
        ast_get_type(loop) == (ASTNodeType) T_WHILE ||
        ast_get_type(loop) == (ASTNodeType) T_UNTIL)
      break;

    /* move up the tree. */
    loop = loop->up;
  }

  /* write based on the node type. */
  if (ntok == T_BREAK) {
    /* check that a loop node was found. */
    if (!loop)
      asterr(node, ERR_OUTSIDE_LOOP, scanner_token_get_name(ntok));

    /* write a simple break and return. */
    W("  break;\n");
  }
  else if (ntok == T_CONTINUE) {
    /* check that a loop node was found. */
    if (!loop)
      asterr(node, ERR_OUTSIDE_LOOP, scanner_token_get_name(ntok));

    /* write a simple continue and return. */
    W("  continue;\n");
  }
  else if (ntok == T_RETURN) {
    /* write a goto statement to skip all remaining function code. */
    W("  goto wrap;\n");
  }

  /* not a control flow statement. */
  return 0;
}

/* write_display(): write a single display statement, or nothing if the
 * specified ast-node is not meant to be displayed.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 */
static void write_display (Compiler c, AST node) {
  /* return if the node should not be displayed. */
  if (!node->node_disp) return;

  /* otherwise, write the display method. */
  W("  if (!object_display(&_z1, %s, \"%s\")) {\n"
    "    Exception _e = (Exception) exceptions_get(_z0);\n"
    "    except_add_call(_z0, _e, \"%s\", \"%s\", %ld);\n"
    "    return (Object) _e;\n"
    "  }\n", S(node),
    ast_get_symbol_type(node) & SYMBOL_TEMP ? "ans" : S(node),
    node->fname, ast_get_func(node), node->line);
}

/* write_statements(): write a statement or statement list.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 */
static void write_statements (Compiler c, AST node) {
  /* do not traverse null nodes. */
  if (!node) return;

  /* get the current node type. */
  const ASTNodeType ntype = ast_get_type(node);
  const ScannerToken ntok = (ScannerToken) ntype;

  /* traverse the current sub-tree based on node type. */
  if (ntype == AST_TYPE_STATEMENTS) {
    /* statement lists: visit each child node. */
    for (int i = 0; i < node->n_down; i++)
      write_statements(c, node->down[i]);

    /* end traversal. */
    return;
  }
  else if (ntype == AST_TYPE_FN_CALL) {
    /* function calls: traverse the second child. */
    write_statements(c, node->down[1]);
  }
  else if (ntype == AST_TYPE_FUNCTION ||
           ntok == T_CLASSDEF) {
    /* functions and class definitions: do not traverse. */
    return;
  }
  else if (write_try(c, node) ||
           write_if(c, node, 0) ||
           write_switch(c, node, 0) ||
           write_for(c, node) ||
           write_while(c, node) ||
           write_until(c, node)) {
    /* try, if, switch, for, while and until blocks: write and return. */
    return;
  }
  else {
    /* all else: traverse all child nodes. */
    for (int i = 0; i < node->n_down; i++)
      write_statements(c, node->down[i]);
  }

  /* write the statement based on its node type. */
  if (write_operation(c, node) ||
      write_concat(c, node) ||
      write_assign(c, node) ||
      write_call(c, node) ||
      write_flow(c, node)) {
    /* write a display handler, if necessary. */
    write_display(c, node);
  }
}

/* write_symbols(): write variable and literal symbol initializers from
 * a symbol table for use within a matte function.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @syms: symbol table to access.
 */
static void write_symbols (Compiler c, Symbols syms) {
  /* loop once to write all input arguments. */
  for (long i = 0; i < syms->n; i++) {
    /* write only input arguments. */
    if (!symbol_has_type(syms, i, SYMBOL_ARGIN)) continue;

    /* write the argin symbol. */
    W("  Object %s = object_list_get((ObjectList) argin, %ld);\n",
      symbol_name(syms, i), i);
  }

  /* loop again to write all non-global, non-temp variables. */
  W("\n");
  for (long i = 0; i < syms->n; i++) {
    /* do not write globals or temps. only write local vars. */
    if (!symbol_has_type(syms, i, SYMBOL_VAR) ||
         symbol_has_type(syms, i, SYMBOL_GLOBAL | SYMBOL_TEMP))
     continue;

    /* write the variable symbol. */
    W("  Object %s = NULL;\n", symbol_name(syms, i));
  }

  /* loop again to write all literals. */
  W("\n");
  for (long i = 0; i < syms->n; i++) {
    /* do not write global symbols. */
    if (symbol_has_type(syms, i, SYMBOL_GLOBAL)) continue;

    /* write based on type. */
    if (symbol_has_type(syms, i, SYMBOL_INT)) {
      /* integer literal. */
      W("  Object %s = (Object) int_new_with_value(&_z1, %ldL);\n",
        symbol_name(syms, i),
        symbol_int(syms, i));
    }
    else if (symbol_has_type(syms, i, SYMBOL_FLOAT)) {
      /* float literal. */
      W("  Object %s = (Object) float_new_with_value(&_z1, %le);\n",
        symbol_name(syms, i),
        symbol_float(syms, i));
    }
    else if (symbol_has_type(syms, i, SYMBOL_COMPLEX)) {
      /* complex literal. */
      W("  Object %s = (Object) \n"
        "    complex_new_with_value(&_z1, %le + %le * I);\n",
        symbol_name(syms, i),
        creal(symbol_complex(syms, i)),
        cimag(symbol_complex(syms, i)));
    }
    else if (symbol_has_type(syms, i, SYMBOL_STRING)) {
      /* string literal. */
      W("  Object %s = (Object) string_new_with_value(&_z1, %s);\n",
        symbol_name(syms, i),
        symbol_string(syms, i));
    }
  }
}

/* write_globals(): write all global declarations.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 */
static void write_globals (Compiler c) {
  /* declare required variables:
   *  @gs: global symbol table.
   *  @i: general-purpose loop counter.
   */
  Symbols gs;
  long i;

  /* get the global symbol table. */
  gs = c->tree->syms;

  /* write the matte include and the global zone allocator. */
  W("\n"
    "#include <matte/matte.h>\n\n");

  /* write the function declarations. */
  for (i = 0; i < gs->n; i++) {
    if (!symbol_has_type(gs, i, SYMBOL_FUNC)) continue;
    W("Object matte_%s (Zone z, Object argin);\n", symbol_name(gs, i));
  }

  /* write the variable declarations. */
  W("\n"
    "ZoneData _zg;\n"
    "bool _zg_init = false;\n\n");
  for (i = 0; i < gs->n; i++) {
    if (!symbol_has_type(gs, i, SYMBOL_VAR) ||
         symbol_has_type(gs, i, SYMBOL_TEMP))
      continue;

    /* write the global variable declaration. */
    W("Object %s = NULL;\n", symbol_name(gs, i));
  }

  /* write the global initializer function. */
  W("\n"
    "void initialize (void) {\n"
    "  if (_zg_init) return;\n"
    "  zone_init(&_zg, %ld);\n"
    "  _zg_init = true;\n"
    "}\n\n",
    gs->n);
}

/* write_functions(): write all user-defined functions.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 */
static void write_functions (Compiler c) {
  /* declare required variables:
   *  @node, @down: general-purpose ast-nodes.
   *  @i, @j: general-purpose loop counters.
   */
  AST node, down;
  int i, j;

  /* FIXME: comment and refactor to use symbol lookups. */
  for (i = 0; i < c->tree->n_down; i++) {
    node = c->tree->down[i];
    if (ast_get_type(node) != AST_TYPE_FUNCTION) continue;

    W("Object matte_%s (Zone _z0, Object argin) {\n"
      "  ZoneData _z1;\n"
      "  zone_init(&_z1, %ld);\n"
      "  Object argout = NULL;\n\n",
      ast_get_string(node->down[1]),
      node->syms->n);

    write_symbols(c, node->syms);

    W("\n");
    write_statements(c, node->down[3]);

    W("\n"
      "wrap:\n");
    down = node->down[0];
    if (!down) {
      W("  argout = object_list_argout(_z0, 0);\n");
    }
    else if (down->n_down) {
      W("  argout = object_list_argout(_z0, %d", down->n_down);
      for (j = 0; j < down->n_down; j++)
        W(", %s", ast_get_string(down->down[j]));
      W(");\n");
    }
    else {
      W("  argout = object_list_argout(_z0, 1, %s);\n", S(down));
    }

    W("  object_free_all(&_z1);\n"
      "  return argout;\n"
      "}\n\n");
  }
}

/* write_main(): write the main function.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 */
static void write_main (Compiler c) {
  /* write a general-purpose main function. */
  W("Object matte_main (Zone _z0, Object argin) {\n"
    "  ZoneData _z1;\n"
    "  zone_init(&_z1, %ld);\n"
    "  initialize();\n\n",
    c->tree->syms->n);

  /* write all global variables and literals. */
  write_symbols(c, c->tree->syms);

  /* write all global statements into the main function. */
  for (int i = 0; i < c->tree->n_down; i++)
    write_statements(c, c->tree->down[i]);

  /* write the end of the main function. */
  W("wrap:\n"
    "  return end;\n"
    "}\n\n");

  /* check if an application entry point is required. */
  if (c->mode != COMPILE_TO_MEM) {
    /* write a simple application entry point. */
    W("int main (int argc, char **argv) {\n"
      "  Object _ao = matte_main(NULL, NULL);\n"
      "  if (IS_EXCEPTION(_ao)) {\n"
      "    object_disp(NULL, _ao);\n"
      "    return 1;\n"
      "  }\n\n"
      "  return 0;\n"
      "}\n\n");
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* compile_to_c(): output generated c source code to a file.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int compile_to_c (Compiler c) {
  /* declare required variables:
   *  @fname: output filename.
   *  @fh: output file handle.
   */
  const char *fname;
  FILE *fh;

  /* check if the output filename is unset. */
  if (!string_get_length(c->fout)) {
    if (object_list_get_length(c->src) >= 1) {
      /* munge the last source file read. */
      string_set_value(c->fout,
        string_get_value((String) object_list_last(c->src)));

      /* either append .c or replace .m with .c */
      if (string_get_length(c->fout) >= 2 &&
          c->fout->data[c->fout->n - 1] == 'm' &&
          c->fout->data[c->fout->n - 2] == '.') {
        /* replace the existing .m suffix. */
        string_set_length(c->fout, string_get_length(c->fout) - 2);
        string_append_value(c->fout, ".c");
      }
      else {
        /* append a new .c suffix. */
        string_append_value(c->fout, ".c");
      }
    }
    else {
      /* use the generic output filename. */
      string_set_value(c->fout, "matte.c");
    }
  }

  /* open the output file. */
  fname = string_get_value(c->fout);
  fh = fopen(fname, "w");

  /* check for errors. */
  if (!fh) fail("unable to open '%s' for writing", fname);

  /* write the source code to the file. */
  fprintf(fh, "%s\n", string_get_value(c->ccode));

  /* close the output file. */
  fclose(fh);

  /* return success. */
  return 1;
}

/* compile_to_exe(): pass generated source code to the system's
 * default c compiler.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int compile_to_exe (Compiler c) {
  /* declare required variables:
   *  @cc: compilation command string.
   *  @ftmp: temporary filename.
   *  @fd: output file descriptor.
   */
  String cc = string_new(NULL, NULL);
  char ftmp[64];
  int fd, ret;

  /* check if the output filename is unset. */
  if (!string_get_length(c->fout)) {
    if (object_list_get_length(c->src) >= 1) {
      /* munge the last source file read. */
      string_set_value(c->fout,
        string_get_value((String) object_list_last(c->src)));

      /* either append .exe or remove .m */
      if (string_get_length(c->fout) >= 2 &&
          c->fout->data[c->fout->n - 1] == 'm' &&
          c->fout->data[c->fout->n - 2] == '.') {
        /* remove the existing .m suffix. */
        string_set_length(c->fout, string_get_length(c->fout) - 2);
      }
      else {
        /* append a new .exe suffix. */
        string_append_value(c->fout, ".exe");
      }
    }
    else {
      /* use the generic output filename. */
      string_set_value(c->fout, "matte");
    }
  }

  /* obtain a temporary file descriptor. */
  strcpy(ftmp, "/tmp/matteXXXXXX.c");
  fd = mkstemps(ftmp, 2);
  if (fd < 0)
    fail(ERR_FOPEN, ftmp);

  /* write the source code to the file. */
  write(fd, c->ccode->data, c->ccode->n * sizeof(char));
  close(fd);

  /* build the compilation command string. */
  string_appendf(cc, "gcc %s %s -o %s -lmatte\n",
                 string_get_value(c->cflags), ftmp,
                 string_get_value(c->fout));

  /* execute the system command. */
  ret = system(string_get_value(cc));
  object_free(NULL, cc);

  /* remove the temporary file. */
  remove(ftmp);

  /* return the result. */
  return (ret == 0);
}

/* compile_to_mem(): pass generated source code to the system's
 * default c compiler, and directly load it into memory for
 * execution.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int compile_to_mem (Compiler c) {
  /* declare required variables:
   *  @cc: compilation command string.
   *  @ftmp?: temporary filenames.
   *  @fd?: output file descriptors.
   */
  String cc = string_new(NULL, NULL);
  char ftmpc[64], ftmpx[64];
  int fdc, fdx, ret;

  /* declare required variables for dynamic loading:
   *  @lib, @sym: pointers to dlopen() and dlsym() results.
   *  @fn: main function pointer.
   *  @results: return values.
   */
  void *lib, *sym;
  Object (*fn) (Zone, Object);
  Object results;

  /* obtain two temporary file descriptors. */
  strcpy(ftmpc, "/tmp/matteXXXXXX.c");
  strcpy(ftmpx, "/tmp/matteXXXXXX.x");
  fdc = mkstemps(ftmpc, 2);
  fdx = mkstemps(ftmpx, 2);

  /* handle file open failures. */
  if (fdc < 0) fail(ERR_FOPEN, ftmpc);
  if (fdx < 0) fail(ERR_FOPEN, ftmpx);

  /* write the source code to the file. */
  write(fdc, c->ccode->data, c->ccode->n * sizeof(char));
  close(fdc);

  /* build the compilation command string. */
  string_appendf(cc, "gcc %s -fPIC -shared -o %s %s -lmatte\n",
                 string_get_value(c->cflags), ftmpx, ftmpc);

  /* execute the system command. */
  close(fdx);
  ret = system(string_get_value(cc));
  object_free(NULL, cc);

  /* check if the command was successful. */
  if (ret == 0) {
    /* yes. load the compiled result into memory. */
    lib = dlopen(ftmpx, RTLD_LAZY);

    /* remove the temporary files. */
    remove(ftmpc);
    remove(ftmpx);

    /* check if the load succeeded. */
    if (!lib)
      fail(ERR_DLOPEN, ftmpx);

    /* gain access to the main function. */
    sym = dlsym(lib, "matte_main");
    if (!sym)
      fail(ERR_DLOPEN, ftmpx);

    /* run the main function. */
    fn = sym;
    results = fn(NULL, NULL);

    /* handle any exceptions. */
    if (IS_EXCEPTION(results))
      object_disp(NULL, results);

    /* free the result and return. */
    object_free(NULL, results);
    return 1;
  }

  /* remove the temporary files. */
  remove(ftmpc);
  remove(ftmpx);

  /* return failure. */
  return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* compiler_type(): return a pointer to the compiler object type.
 */
ObjectType compiler_type (void) {
  /* return the struct pointer. */
  return &Compiler_type;
}

/* compiler_new(): allocate a new matte compiler.
 *
 * arguments:
 *  @z: zone allocator to completely ignore.
 *  @args: constructor arguments.
 *
 * returns:
 *  newly allocated and initialized matte compiler.
 */
Compiler compiler_new (Zone z, Object args) {
  /* allocate a new compiler. */
  Compiler c = (Compiler) object_alloc(NULL, &Compiler_type);
  if (!c)
    return NULL;

  /* get the matte path environment variable. */
  const char *pathdata = getenv(MATTEPATH_ENV_STRING);

  /* check if the path contains directories. */
  if (pathdata) {
    /* allocate temporary strings. */
    String pathstr = string_new_with_value(NULL, pathdata);
    String pathsep = string_new_with_value(NULL, ":");

    /* split the path string by delimiters and free the path string. */
    c->path = string_split(NULL, pathstr, pathsep);

    /* free the temporary strings. */
    object_free(NULL, pathstr);
    object_free(NULL, pathsep);
  }
  else {
    /* allocate an empty path list. */
    c->path = object_list_new(NULL, NULL);
  }

  /* allocate an empty source list. */
  c->src = object_list_new(NULL, NULL);

  /* allocate the associated parser. */
  c->par = parser_new(NULL, NULL);
  if (!c->par)
    return NULL;

  /* initialize the abstract syntax tree. */
  c->tree = NULL;

  /* initialize the mode and filename. */
  c->mode = COMPILE_TO_MEM;
  c->fout = string_new(NULL, NULL);
  c->cflags = string_new(NULL, NULL);

  /* initialize the c code string. */
  c->ccode = string_new(NULL, NULL);

  /* initialize the catch variables. */
  c->catching = false;
  c->cvar = NULL;
  c->cidx = 0;
  NEW_LABEL;

  /* initialize the error count. */
  c->err = 0L;

  /* return the new compiler. */
  return c;
}

/* compiler_delete(): free all memory associated with a matte compiler.
 *
 * arguments:
 *  @z: zone allocator to completely ignore.
 *  @c: matte compiler to free.
 */
void compiler_delete (Zone z, Compiler c) {
  /* return if the compiler is null. */
  if (!c)
    return;

  /* free the strings in the path list. */
  for (int i = 0; i < object_list_get_length(c->path); i++)
    object_free(NULL, object_list_get(c->path, i));

  /* free the strings in the source list. */
  for (int i = 0; i < object_list_get_length(c->src); i++)
    object_free(NULL, object_list_get(c->src, i));

  /* free the path and source lists. */
  object_free(NULL, c->path);
  object_free(NULL, c->src);

  /* free the parser. this also frees the tree. */
  object_free(NULL, c->par);

  /* free the output filename and flags strings. */
  object_free(NULL, c->fout);
  object_free(NULL, c->cflags);

  /* free the c code string. */
  object_free(NULL, c->ccode);
}

/* compiler_set_mode(): set the output mode of a compiler.
 *
 * arguments:
 *  @c: matte compiler to modify.
 *  @mode: new compilation mode to set.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_set_mode (Compiler c, CompilerMode mode) {
  /* validate the input arguments. */
  if (!c)
    fail(ERR_INVALID_ARGIN);

  /* store the new mode. */
  c->mode = mode;

  /* if the mode is to compile to memory, clear the filename string. */
  if (c->mode == COMPILE_TO_MEM)
    string_set_value(c->fout, "");

  /* return success. */
  return 1;
}

/* compiler_set_outfile(): set the output filename of a compiler.
 *
 * arguments:
 *  @c: matte compiler to modify.
 *  @fname: new output filename string.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_set_outfile (Compiler c, const char *fname) {
  /* validate the input arguments. */
  if (!c || !fname)
    fail(ERR_INVALID_ARGIN);

  /* check that we're not trying to set a file for memory compilation. */
  if (c->mode == COMPILE_TO_MEM)
    fail("matte:compiler", "mode does not support output files");

  /* store the output filename. */
  return string_set_value(c->fout, fname);
}

/* compiler_add_cflag(): add a c compiler flag to a matte compiler.
 *
 * arguments:
 *  @c: matte compiler to modify.
 *  @str: compiler flag string to add.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_add_cflag (Compiler c, const char *str) {
  /* validate the input arguments. */
  if (!c || !str)
    fail(ERR_INVALID_ARGIN);

  /* append the flag to the flags string. */
  return (string_append_value(c->cflags, " ") &&
          string_append_value(c->cflags, str));
}

/* compiler_add_path(): add a pathname string to a matte compiler.
 *
 * arguments:
 *  @c: matte compiler to modify.
 *  @fname: path filename string to add.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_add_path (Compiler c, const char *fname) {
  /* validate the input arguments. */
  if (!c || !fname)
    fail(ERR_INVALID_ARGIN);

  /* append the path string into the list. */
  Object sobj = (Object) string_new_with_value(NULL, fname);
  return object_list_append(c->path, sobj);
}

/* compiler_add_file(): read a file using a matte compiler.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @fname: filename string to open.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_add_file (Compiler c, const char *fname) {
  /* validate the input arguments. */
  if (!c || !fname)
    fail(ERR_INVALID_ARGIN);

  /* set the string in the associated parser. */
  if (!parser_set_file(c->par, fname)) {
    /* on errors, store the error count and reset the tree. */
    c->err += c->par->err;
    c->tree = NULL;
    return 0;
  }

  /* add the filename to the source list. */
  Object sobj = (Object) string_new_with_value(NULL, fname);
  object_list_append(c->src, sobj);

  /* get the current tree pointer and return success. */
  c->tree = c->par->tree;
  return 1;
}

/* compiler_add_string(): read a string using a matte compiler.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @str: buffer string to read.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_add_string (Compiler c, const char *str) {
  /* validate the input arguments. */
  if (!c || !str)
    fail(ERR_INVALID_ARGIN);

  /* set the string in the associated parser. */
  if (!parser_set_string(c->par, str)) {
    c->err += c->par->err;
    return 0;
  }

  /* get the current tree pointer and return success. */
  c->tree = c->par->tree;
  return 1;
}

/* compiler_execute(): initiate compilation of all syntax tree content
 * contained by a compiler.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int compiler_execute (Compiler c) {
  /* check that the compiler is ready for compilation. */
  if (!c || !c->tree)
    fail(ERR_COMPILER_EMPTY);

  /* simplify horizontal and vertical concatenations. */
  simplify_concats(c->tree);

  /* initialize the symbol tables inside the syntax tree. */
  if (!init_symbols(c, c->tree))
    return 0;

  /* resolve symbols required by the syntax tree. */
  if (!resolve_symbols(c, c->tree))
    return 0;

  /* write global symbols and functions. */
  write_globals(c);
  write_functions(c);

  /* write the main function. */
  write_main(c);

  /* perform an explicit check for exceptions. */
  if (exceptions_check())
    fail(ERR_COMPILER_GENERAL);

  /* determine the compilation mode. */
  switch (c->mode) {
    /* supported modes: */
    case COMPILE_TO_C:   return compile_to_c(c);
    case COMPILE_TO_EXE: return compile_to_exe(c);
    case COMPILE_TO_MEM: return compile_to_mem(c);

    /* any other mode: */
    default: fail(ERR_COMPILER_MODE);
  }

  /* return success. */
  return 1;
}

/* Compiler_type: object type structure for matte compilers.
 */
struct _ObjectType Compiler_type = {
  "Compiler",                                    /* name       */
  sizeof(struct _Compiler),                      /* size       */
  0,                                             /* precedence */

  (obj_constructor) compiler_new,                /* fn_new    */
  NULL,                                          /* fn_copy   */
  (obj_destructor)  compiler_delete,             /* fn_delete */
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

