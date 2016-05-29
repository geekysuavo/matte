
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

/* E(): macro function for obtaining an error string from a matte ast-node.
 */
#define E(nd, ...) ast_error_string(c, nd, true, __VA_ARGS__)

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
  /* @ntype: node type of the matte ast-node containing the operation.
   * @noper: number of downfield operands in the operation.
   * @fstr: function name of the overloadable operation.
   */
  ASTNodeType ntype;
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
 *  @root: root of the matte ast being processed.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int init_symbols (Compiler c, AST node, AST root) {
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
  if (ntype == AST_TYPE_FUNCTION) {
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
    if (!init_symbols(c, node->down[3], root))
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
    if (!init_symbols(c, node->down[1], root))
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
      if (!init_symbols(c, node->down[i], root))
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
 *  @root: root of the matte ast being processed.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int resolve_symbols (Compiler c, AST node, AST root) {
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
        if (super->syms->sym_type[sid - 1] & SYMBOL_FUNC) {
          /* get the symbol table for the node. */
          syms = ast_get_symbols(node);
          if (!syms) return 0;

          /* register symbols for the function arguments and results. */
          if (!symbols_add(syms, SYMBOL_VAR, "args") ||
              !symbols_add(syms, SYMBOL_VAR, "rets"))
            return 0;

          /* determine the call mode of the function. */
          if (ast_get_type(node->up) == (ASTNodeType) T_ASSIGN) {
            /* handle the assignment using a function call. */
            node->up->node_type = AST_TYPE_FN_CALL;
          }
          else {
            /* insert an assignment for the function call. */
            super = ast_new_with_type(T_IDENT);
            super = ast_new_with_parms(AST_TYPE_FN_CALL, false, super);
            if (!ast_slip(node, super))
              return 0;

            /* register a temporary symbol for the result. */
            if (!ast_add_symbol(super->down[0], super->down[0],
                                SYMBOL_TEMP_VAR))
              return 0;

            /* also store the temporary symbol at the root. */
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
    if (!resolve_symbols(c, node->down[3], root))
      return 0;

    /* return success. */
    return 1;
  }

  /* traverse further into the tree. */
  for (i = 0; i < node->n_down; i++) {
    if (!resolve_symbols(c, node->down[i], root))
      return 0;
  }

  /* return success. */
  return 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* write_except_handler(): write exception handler code based on the
 * location of a node within the syntax tree.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: ast-node to write code for.
 *  @sym: symbol name to check for exceptions.
 *  @is_main: whether the handler is within main().
 */
static void write_except_handler (Compiler c, AST node,
                                  const char *sym, bool is_main) {
  /* determine which kind of exception handler to write. */
  if (is_main) {
    /* write an exception propagator for functions an matte_main(). */
    W("  EXCEPT_HANDLE_MAIN(%s, \"%s\", \"%s\", %ld);\n",
      sym, node->fname, ast_get_func(node), node->line);
  }
  else {
    /* write an exception terminator for main(). */
    W("  EXCEPT_HANDLE(%s, \"%s\", \"%s\", %ld);\n",
      sym, node->fname, ast_get_func(node), node->line);
  }
}

/* write_statements(): write a statement or statement list.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 *  @node: matte ast-node to process.
 *  @is_main: whether the statements are within main().
 */
static void write_statements (Compiler c, AST node, bool is_main) {
  /* FIXME: comment and refactor. */
  const char *sname;
  AST down;
  int i;

  if (!node) return;

  const ASTNodeType ntype = ast_get_type(node);
  const ScannerToken ntok = (ScannerToken) ntype;

  if (ntype == AST_TYPE_STATEMENTS) {
    for (i = 0; i < node->n_down; i++)
      write_statements(c, node->down[i], is_main);

    return;
  }
  else if (ntype == AST_TYPE_FN_CALL) {
    write_statements(c, node->down[1], is_main);
  }
  else if (ntype == AST_TYPE_FUNCTION) {
    return;
  }
  else {
    for (i = 0; i < node->n_down; i++)
      write_statements(c, node->down[i], is_main);
  }

  for (i = 0; operators[i].fstr; i++) {
    if (ntype == operators[i].ntype &&
        node->n_down == operators[i].noper)
      break;
  }

  if (operators[i].fstr) {
    if (node->n_down == 1) {
      W("  Object %s = %s(&_z1, %s);\n",
        S(node), operators[i].fstr,
        S(node->down[0]));
    }
    else if (node->n_down == 2) {
      W("  Object %s = %s(&_z1, %s, %s);\n",
        S(node), operators[i].fstr,
        S(node->down[0]),
        S(node->down[1]));
    }
    else if (node->n_down == 3) {
      W("  Object %s = %s(&_z1, %s, %s, %s);\n",
        S(node), operators[i].fstr,
        S(node->down[0]),
        S(node->down[1]),
        S(node->down[2]));
    }

    write_except_handler(c, node, S(node), is_main);
  }
  else if (ntype == AST_TYPE_ROW) {
    W("  Object %s = object_horzcat(&_z1, %d", S(node), node->n_down);
    for (i = 0; i < node->n_down; i++)
      W(", %s", S(node->down[i]));
    W(");\n");

    write_except_handler(c, node, S(node), is_main);
  }
  else if (ntype == AST_TYPE_COLUMN) {
    W("  Object %s = object_vertcat(&_z1, %d", S(node), node->n_down);
    for (i = 0; i < node->n_down; i++)
      W(", %s", S(node->down[i]));
    W(");\n");

    write_except_handler(c, node, S(node), is_main);
  }
  else if (ntok == T_ASSIGN) {
    W("  %s = %s;\n", S(node), S(node->down[1]));
  }
  else if (ntype == AST_TYPE_FN_CALL) {
    down = node->down[1];
    if (down->n_down == 1 &&
        ast_get_type(down->down[0]) == (ASTNodeType) T_PAREN_OPEN) {
      down = down->down[0];
      W("  args = object_list_argout(&_z1, %d", down->n_down);
      for (i = 0; i < down->n_down; i++)
        W(", %s", S(down->down[i]));
      W(");\n");
    }
    else {
      W("  args = (Object) object_list_new(&_z1, NULL);\n");
    }

    down = node->down[1];
    W("  rets = matte_%s(&_z1, args);\n", S(down));
    write_except_handler(c, down, "rets", is_main);

    down = node->down[0];
    if (ast_get_type(down) == (ASTNodeType) T_IDENT) {
      sname = S(down);
      W("  %s%s = object_list_get((ObjectList) rets, 0);\n",
        sname[0] == '_' ? "Object " : "", sname);
    }
    else if (ast_get_type(down) == AST_TYPE_ROW) {
      for (i = 0; i < down->n_down; i++)
        W("  %s = object_list_get((ObjectList) rets, %d);\n",
          S(down->down[i]), i);
    }

    W("  object_free(&_z1, args);\n");
    W("  object_free(&_z1, rets);\n");
  }

  if (node->node_disp) {
    W("  if (!object_disp(&_z1, %s, \"%s\")) {\n", S(node), S(node));

    if (is_main) {
      W("    Exception _e = (Exception) exceptions_get(&_z1);\n"
        "    except_add_call(&_z1, _e, \"%s\", \"%s\", %ld);\n"
        "    object_disp(&_z1, (Object) _e, \"e\");\n"
        "    return 1;\n", node->fname, ast_get_func(node), node->line);
    }
    else {
      W("    Exception _e = (Exception) exceptions_get(_z0);\n"
        "    except_add_call(_z0, _e, \"%s\", \"%s\", %ld);\n"
        "    return (Object) _e;\n",
        node->fname, ast_get_func(node), node->line);
    }

    W("  }\n");
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

  /* write the matte include. */
  W("\n#include <matte/matte.h>\n\n");

  /* write the function declarations. */
  for (i = 0; i < gs->n; i++) {
    if (!(gs->sym_type[i] & SYMBOL_FUNC)) continue;
    W("Object matte_%s (Zone z, Object argin);\n", gs->sym_name[i]);
  }

  /* write the variable declarations. */
  W("\n");
  for (i = 0; i < gs->n; i++) {
    if (!(gs->sym_type[i] & SYMBOL_VAR) ||
        gs->sym_name[i][0] == '_')
     continue;

    /* write the global variable declaration. */
    W("Object %s = NULL;\n", gs->sym_name[i]);
  }
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

    W("\n");
    W("Object matte_%s (Zone _z0, Object argin) {\n"
      "  ZoneData _z1;\n"
      "  zone_init(&_z1, %ld);\n",
      ast_get_string(node->down[1]),
      node->syms->n);

    write_symbols(c, node->syms);

    W("\n");
    write_statements(c, node->down[3], false);

    W("\n");
    down = node->down[0];
    if (!down) {
      W("  return (Object) object_list_new(_z0, NULL);\n");
    }
    else if (down->n_down) {
      W("  return object_list_argout(_z0, %d", down->n_down);
      for (j = 0; j < down->n_down; j++)
        W(", %s", ast_get_string(down->down[j]));
      W(");\n");
    }
    else {
      W("  return object_list_argout(_z0, 1, %s);\n", S(down));
    }

    W("}\n");
  }
}

/* write_main(): write the main function.
 *
 * arguments:
 *  @c: matte compiler to utilize.
 */
static void write_main (Compiler c) {
  /* declare required variables:
   *  @node: general-purpose ast-node.
   *  @i: general-purpose loop counter.
   */
  AST node;
  int i, n;

  /* check if compile-to-memory mode is enabled. */
  if (c->mode == COMPILE_TO_MEM) {
    /* write a general-purpose main function. */
    W("\nObject matte_main (Zone _z0, Object argin) {\n");
  }
  else {
    /* write an application entry point. */
    W("\nint main (int argc, char **argv) {\n");
  }

  /* write the zone allocator initialization. */
  W("  ZoneData _z1;\n"
    "  zone_init(&_z1, %ld);\n",
    c->tree->syms->n);

  /* write all global variables and literals. */
  write_symbols(c, c->tree->syms);

  /* write all global statements into the main function. */
  for (i = 0; i < c->tree->n_down; i++)
    write_statements(c, c->tree->down[i], c->mode != COMPILE_TO_MEM);

  /* write the end of the main function. */
  if (c->mode == COMPILE_TO_MEM)
    W("\n  return end;\n}\n");
  else
    W("\n  return 0;\n}\n");
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
      object_disp(NULL, results, "e");

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

  /* initialize the c code and error strings. */
  c->ccode = string_new(NULL, NULL);
  c->cerr = NULL;

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

  /* free the c code and error strings. */
  object_free(NULL, c->ccode);
  free(c->cerr);
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
  /* simplify horizontal and vertical concatenations. */
  simplify_concats(c->tree);

  /* initialize the symbol tables inside the syntax tree. */
  if (!init_symbols(c, c->tree, c->tree))
    return 0;

  /* resolve symbols required by the syntax tree. */
  if (!resolve_symbols(c, c->tree, c->tree))
    return 0;

  /* write global symbols and functions. */
  write_globals(c);
  write_functions(c);

  /* write the main function. */
  write_main(c);

  /* determine the compilation mode. */
  switch (c->mode) {
    /* supported modes: */
    case COMPILE_TO_C:   return compile_to_c(c);
    case COMPILE_TO_EXE: return compile_to_exe(c);
    case COMPILE_TO_MEM: return compile_to_mem(c);

    /* any other mode: */
    default: fail("matte:compiler", "unsupported compilation type");
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

