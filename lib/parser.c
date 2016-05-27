
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the parser header. */
#include <matte/parser.h>

/* PARSE_RULE: macro to begin a new parsing rule (function).
 */
#define PARSE_RULE(rule) \
  static AST parse_##rule (Parser p) { \
    const char *rname = #rule; \
    AST node = NULL;

/* PARSE_NEWLINES: macro to ignore any newline characters.
 */
#define PARSE_NEWLINES \
  while (match(p, T_EOL)) next(p);

/* PARSE_ERR: macro to report a generic syntax error during parsing.
 */
#define PARSE_ERR \
  { errorfn(p, "syntax error in %s", rname); \
    return NULL; }

/* PARSE_ERR_LVALUE: macro to report an invalid lvalue during parsing.
 */
#define PARSE_ERR_LVALUE \
  { errorfn(p, "invalid left hand side in %s", rname); \
    return NULL; }

/* PARSE_ERR_RANGE: macro to report an invalid range during parsing.
 */
#define PARSE_ERR_RANGE \
  { errorfn(p, "invalid %s expression", rname); \
    return NULL; }

/* PARSE_ERR_EXISTS: macro to report a pre-existing tree node during parsing.
 */
#define PARSE_ERR_EXISTS(tok) \
  { errorfn(p, "class %s have already been defined", \
            scanner_token_get_name(tok)); \
    object_free((Object) node); \
    return NULL; }

/* PARSE_ERR_MISSING: macro to report a missing rule during parsing.
 */
#define PARSE_ERR_MISSING(s) \
  { errorfn(p, "missing %s in %s", s, rname); \
    object_free((Object) node); \
    return NULL; }

/* PARSE_ERR_MISSING_TOKEN: macro to report a missing token during parsing.
 */
#define PARSE_ERR_MISSING_TOKEN(tk) \
  { errorfn(p, "expected %s, got %s in %s", \
            scanner_token_get_name(tk), \
            scanner_token_get_name(p->tok), rname); \
    object_free((Object) node); \
    return NULL; }

/* PARSE_REQUIRE: macro to assert that a given token exists at the
 * current lookahead position during parsing.
 */
#define PARSE_REQUIRE(tok) \
  if (!accept(p, tok)) \
    PARSE_ERR_MISSING_TOKEN(tok)

/* PARSE_REQUIRE_STMT_END: macro to assert that a stmt_end rule is matched
 * at the current lookahead position during parsing.
 */
#define PARSE_REQUIRE_STMT_END \
  if (!(accept(p, T_SEMI) || accept(p, T_COMMA) || accept(p, T_EOL))) \
    PARSE_ERR_MISSING("end of statement"); \
  PARSE_NEWLINES;

/* SAVE_CONTEXT: macro to save the current scanner context into local
 * variables, which may be accessed to set ast-node context at a later
 * point in a parse rule.
 */
#define SAVE_CONTEXT \
  const char *ctx_fname = scanner_get_filename(p->scan); \
  const long ctx_line = scanner_get_lineno(p->scan); \
  const long ctx_pos = scanner_get_pos(p->scan);

/* SAVED_CONTEXT: macro to recall the local variables created by the
 * SAVE_CONTEXT macro.
 */
#define SAVED_CONTEXT \
  ctx_fname, ctx_line, ctx_pos

/* parsing function declarations: */

static AST parse_stmts (Parser p);
static AST parse_args (Parser p);
static AST parse_expr (Parser p);
static AST parse_name (Parser p);

/* === */

/* next(): load a new lookahead token into a matte parser.
 *
 * arguments:
 *  @p: matte parser to access.
 */
static inline void next (Parser p) {
  /* read a single token of lookahead into the parser. */
  p->tok = scanner_next(p->scan);
}

/* match(): check the current lookahead token of a matte parser.
 *
 * arguments:
 *  @p: matte parser to access.
 *  @tok: token to check against the lookhead.
 *
 * returns:
 *  boolean indicating token match.
 */
static inline bool match (Parser p, ScannerToken tok) {
  /* if the lookahead token is invalid, read a fresh one. */
  if (!p->tok)
    next(p);

  /* return whether or not the token matches the lookahead. */
  return (p->tok == tok);
}

/* accept(): skip the current lookahead pointer if it matches expectation.
 *
 * arguments:
 *  @p: matte parser to access.
 *  @tok: token to check against the lookahead.
 *
 * returns:
 *  boolean indicating token match. upon token match, a new token is
 *  read into the parser's lookahead.
 */
static inline bool accept (Parser p, ScannerToken tok) {
  /* check for a match. */
  if (match(p, tok)) {
    /* advance the lookahead and return true. */
    next(p);
    return true;
  }

  /* return false. */
  return false;
}

/* ast_set_context(): set the source context of a matte ast-node using
 * data contained in a matte parser's associated scanner.
 *
 * arguments:
 *  @p: matte parser to access. must be non-null.
 *  @node: ast-node to modify. must be non-null.
 */
static inline void ast_set_context (Parser p, AST node) {
  /* set the filename, line number and position. */
  ast_set_source(node, scanner_get_filename(p->scan),
                       scanner_get_lineno(p->scan),
                       scanner_get_pos(p->scan));
}

/* valid_lvalue(): check that a given sub-tree is a valid left-hand side
 * of an expression.
 *
 * arguments:
 *  @node: matte ast-node to validate.
 *
 * returns:
 *  integer indicating whether (1) or not (0) the sub-tree is a valid
 *  left-hand side expression.
 */
static int valid_lvalue (AST node) {
  /* fail on null tree nodes. */
  if (!node) return 0;

  /* get the node type. */
  const ASTNodeType ntype = ast_get_type(node);

  /* check the l-value based on it's root type. */
  if (ntype == (ASTNodeType) T_IDENT) {
    /* validate the qualifiers on the identifier. */
    for (int i = 0; i < node->n_down; i++) {
      if (!valid_lvalue(node->down[i]))
        return 0;
    }

    /* return success. */
    return 1;
  }
  else if (ntype == (ASTNodeType) T_AS ||
           ntype == (ASTNodeType) T_POINT) {
    /* force the sub-node to be an identifier. */
    return (node->n_down && node->down[0] &&
            ast_get_type(node->down[0]) == (ASTNodeType) T_IDENT);
  }
  else if (ntype == AST_TYPE_COLUMN) {
    /* force the sub-tree to be a valid row of identifiers. */
    return (node->n_down == 1 &&
            ast_get_type(node->down[0]) == AST_TYPE_ROW &&
            valid_lvalue(node->down[0]));
  }
  else if (ntype == AST_TYPE_ROW) {
    /* validate the identifiers in the row. */
    for (int i = 0; i < node->n_down; i++) {
      /* fail on null nodes. */
      if (!node->down[i]) return 0;

      /* check that identifiers are un-qualified. */
      if (ast_get_type(node->down[i]) != (ASTNodeType) T_IDENT ||
          node->down[i]->n_down)
        return 0;
    }

    /* return success. */
    return 1;
  }
  else if (ntype == (ASTNodeType) T_PAREN_OPEN ||
           ntype == (ASTNodeType) T_BRACE_OPEN) {
    /* validate the identifiers in the array/cell subscripting. */
    for (int i = 0; i < node->n_down; i++) {
      /* fail on null nodes. */
      if (!node->down[i]) return 0;

      /* check allowed node types. */
      if (ast_get_type(node->down[i]) != (ASTNodeType) T_IDENT &&
          ast_get_type(node->down[i]) != (ASTNodeType) T_COLON &&
          ast_get_type(node->down[i]) != (ASTNodeType) T_INT &&
          ast_get_type(node->down[i]) != (ASTNodeType) T_END)
        return 0;

      /* check that identifiers are un-qualified. */
      if (ast_get_type(node->down[i]) == (ASTNodeType) T_IDENT &&
          node->down[i]->n_down)
        return 0;
    }

    /* return success. */
    return 1;
  }

  /* return failure. */
  return 0;
}

/* errorfn(): print an immediate parser error message and increment the
 * error count of a parser.
 *
 * arguments:
 *  @p: matte parser to access and modify.
 *  @format: printf-style error message format string.
 *  @...: arguments corresponding to the format string.
 */
static void errorfn (Parser p, const char *format, ...) {
  /* declare required variables:
   *  @vl: variable-length argument list for message printing.
   *  @line: line string from the associated scanner.
   */
  va_list vl;
  char *line;

  /* reset the end-valid status of the parser. */
  p->end_valid = false;

  /* only print the first (and likely most relevant) error message. */
  if (p->err) {
    p->err++;
    return;
  }

  /* flush standard output. */
  fflush(stdout);

  /* print the initial portion of the error message. */
  fprintf(stderr, "%s:%ld: error: ",
          scanner_get_filename(p->scan),
          scanner_get_lineno(p->scan));

  /* print the custom error message and a newline. */
  va_start(vl, format);
  vfprintf(stderr, format, vl);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(vl);

  /* obtain and print a line string from the scanner. */
  line = scanner_get_linestr(p->scan);
  if (line) {
    fprintf(stderr, "%s", line);
    free(line);
  }

  /* increment the parser error count. */
  p->err++;
}

/* === */

/* row : expr row_next ;
 *
 * row_next
 *  : T_COMMA row
 *  | %empty
 *  ;
 */
PARSE_RULE (row)
  SAVE_CONTEXT;
  node = parse_expr(p);
  if (node) {
    node = ast_new_with_parms(AST_TYPE_ROW, false, node);
    ast_set_source(node, SAVED_CONTEXT);
  }
  else
    return NULL;

  while (accept(p, T_COMMA)) {
    ast_add_down(node, parse_expr(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* column : expr column_next ;
 *
 * column_next
 *  : T_SEMI column
 *  | row_next
 *  ;
 */
PARSE_RULE (column)
  SAVE_CONTEXT;
  node = parse_row(p);
  if (node) {
    node = ast_new_with_parms(AST_TYPE_COLUMN, false, node);
    ast_set_source(node, SAVED_CONTEXT);
  }
  else
    return NULL;

  while (accept(p, T_SEMI)) {
    ast_add_down(node, parse_row(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* qualifier
 *  : T_AS T_IDENT
 *  | T_POINT T_IDENT
 *  | T_PAREN_OPEN row T_PAREN_CLOSE
 *  | T_BRACE_OPEN row T_BRACE_CLOSE
 *  | %empty
 *  ;
 */
PARSE_RULE (qualifier)
  if (accept(p, T_AS)) {
    if (!match(p, T_IDENT))
      PARSE_ERR_MISSING_TOKEN(T_IDENT);

    node = ast_new_with_data(p, NULL);
    return ast_new_with_parms(T_AS, false, node);
  }
  else if (accept(p, T_POINT)) {
    if (!match(p, T_IDENT))
      PARSE_ERR_MISSING_TOKEN(T_IDENT);

    node = ast_new_with_data(p, NULL);
    return ast_new_with_parms(T_POINT, false, node);
  }
  else if (accept(p, T_PAREN_OPEN)) {
    if (accept(p, T_PAREN_CLOSE))
      return NULL;

    p->end_valid = true;
    node = parse_row(p);
    if (!node)
      PARSE_ERR;

    PARSE_REQUIRE(T_PAREN_CLOSE);
    p->end_valid = false;

    ast_set_type(node, T_PAREN_OPEN);
    return node;
  }
  else if (accept(p, T_BRACE_OPEN)) {
    node = parse_row(p);
    if (!node)
      PARSE_ERR;

    PARSE_REQUIRE(T_BRACE_CLOSE);

    ast_set_type(node, T_BRACE_OPEN);
    return node;
  }

  return NULL;
}

/* name : T_IDENT quals ;
 * quals : qualifier quals_next ;
 * quals_next : quals | %empty ;
 */
PARSE_RULE (name)
  if (match(p, T_IDENT)) {
    node = ast_new_with_data(p, NULL);
  }
  else
    return NULL;

  do {
    ast_add_down(node, parse_qualifier(p));
  } while (ast_last(node));

  ast_shrink_down(node);
  return node;
}

/* matrix : T_BRACK_OPEN column T_BRACK_CLOSE ;
 */
PARSE_RULE (matrix)
  if (!accept(p, T_BRACK_OPEN))
    return NULL;

  node = parse_column(p);
  if (!node)
    node = ast_new_with_type(AST_TYPE_EMPTY);

  PARSE_REQUIRE(T_BRACK_CLOSE);
  return node;
}

/* asterisk : T_AS asterisk_end ;
 *
 * asterisk_end
 *  : T_IDENT
 *  | T_PAREN_OPEN args T_PAREN_CLOSE lgor
 *  ;
 */
PARSE_RULE (asterisk)
  if (!accept(p, T_AS))
    return NULL;

  if (match(p, T_IDENT)) {
    node = ast_new_with_type(AST_TYPE_FN_HANDLE);
    ast_set_context(p, node);
    next(p);
  }
  else if (accept(p, T_PAREN_OPEN)) {
    node = ast_new_with_type(AST_TYPE_FN_ANONY);
    ast_add_down(node, parse_args(p));
    PARSE_REQUIRE(T_PAREN_CLOSE);
    ast_add_down(node, parse_expr(p));
  }

  return node;
}

/* value
 *  : name
 *  | matrix
 *  | asterisk
 *  | T_INT
 *  | T_FLOAT
 *  | T_COMPLEX
 *  | T_STRING
 *  | T_PAREN_OPEN expr T_PAREN_CLOSE
 *  | T_COLON  << only valid within parenthetical qualifiers >>
 *  | end      << only valid within parenthetical qualifiers >>
 *  ;
 */
PARSE_RULE (value)
  if (match(p, T_IDENT)) {
    node = parse_name(p);
  }
  else if (match(p, T_BRACK_OPEN)) {
    node = parse_matrix(p);
  }
  else if (match(p, T_AS)) {
    node = parse_asterisk(p);
    if (!node)
      PARSE_ERR;
  }
  else if (match(p, T_INT) ||
           match(p, T_FLOAT) ||
           match(p, T_COMPLEX) ||
           match(p, T_STRING)) {
    node = ast_new_with_data(p, NULL);
  }
  else if (accept(p, T_PAREN_OPEN)) {
    node = parse_expr(p);
    if (!node)
      PARSE_ERR;

    PARSE_REQUIRE(T_PAREN_CLOSE);
  }
  else if (p->end_valid && (match(p, T_END) || match(p, T_COLON))) {
    node = ast_new_with_data(p, NULL);
  }

  return node;
}

/* === */

/* prefix
 *  : T_INC name
 *  | T_DEC name
 *  | value
 *  ;
 */
PARSE_RULE (prefix)
  if (match(p, T_INC) || match(p, T_DEC)) {
    node = ast_new_with_data(p, NULL);
    ast_add_down(node, parse_name(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("name");
  }
  else {
    node = parse_value(p);
  }

  return node;
}

/* === */

/* power : prefix power_end ;
 *
 * power_end
 *  : T_HTR
 *  | T_TR
 *  | T_POW prefix
 *  | T_ELEM_POW prefix
 *  | %empty
 *  ;
 */
PARSE_RULE (power)
  node = parse_prefix(p);
  if (!node)
    return NULL;

  if (match(p, T_HTR) || match(p, T_TR)) {
    node = ast_new_with_data(p, node);
  }
  else if (match(p, T_POW) || match(p, T_ELEM_POW)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_prefix(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* unary
 *  : T_PLUS power
 *  | T_MINUS power
 *  | T_NOT power
 *  | power
 *  ;
 */
PARSE_RULE (unary)
  if (accept(p, T_PLUS)) {
    node = parse_power(p);
  }
  else if (match(p, T_MINUS) || match(p, T_NOT)) {
    node = ast_new_with_data(p, NULL);
    ast_add_down(node, parse_power(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }
  else {
    node = parse_power(p);
  }

  return node;
}

/* postfix : unary postfix_end ;
 *
 * postfix_end
 *  : T_INC
 *  | T_DEC
 *  | %empty
 *  ;
 */
PARSE_RULE (postfix)
  node = parse_unary(p);
  if (!node)
    return NULL;

  if (match(p, T_INC) || match(p, T_DEC)) {
    node = ast_new_with_data(p, node);
  }

  return node;
}

/* === */

/* mult : postfix mult_next ;
 *
 * mult_next
 *  : T_MUL mult
 *  | T_DIV mult
 *  | T_LDIV mult
 *  | T_ELEM_MUL mult
 *  | T_ELEM_DIV mult
 *  | T_ELEM_LDIV mult
 *  | %empty
 *  ;
 */
PARSE_RULE (mult)
  node = parse_postfix(p);
  if (!node)
    return NULL;

  while (match(p, T_MUL) || match(p, T_ELEM_MUL) ||
         match(p, T_DIV) || match(p, T_ELEM_DIV) ||
         match(p, T_LDIV) || match(p, T_ELEM_LDIV)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_postfix(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* add : mult add_next ;
 *
 * add_next
 *  : T_PLUS add
 *  | T_MINUS add
 *  | %empty
 *  ;
 */
PARSE_RULE (add)
  node = parse_mult(p);
  if (!node)
    return NULL;

  while (match(p, T_PLUS) || match(p, T_MINUS)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_mult(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* range : add range_end ;
 *
 * range_end
 *  : T_COLON range
 *  | %empty
 *  ;
 */
PARSE_RULE (range)
  node = parse_add(p);
  if (!node)
    return NULL;

  while (match(p, T_COLON)) {
    if (ast_get_type(node) != (ASTNodeType) T_COLON) {
      node = ast_new_with_parms(T_COLON, false, node);
      ast_set_context(p, node);
    }

    next(p);
    ast_add_down(node, parse_add(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  if (ast_get_type(node) == (ASTNodeType) T_COLON) {
    if (node->n_down == 2) {
      ast_add_down(node, node->down[1]);
      node->down[1] = ast_new_with_type(T_INT);
      ast_set_int(node->down[1], 1L);
      node->down[1]->up = node;
    }
    else if (node->n_down != 3)
      PARSE_ERR_RANGE;
  }

  return node;
}

/* === */

/* reln : range reln_next ;
 *
 * reln_next
 *  : T_LT reln
 *  | T_LE reln
 *  | T_EQ reln
 *  | T_GE reln
 *  | T_GT reln
 *  | T_NE reln
 *  | %empty
 *  ;
 */
PARSE_RULE (reln)
  node = parse_range(p);
  if (!node)
    return NULL;

  while (match(p, T_LT) || match(p, T_LE) ||
         match(p, T_GT) || match(p, T_GE) ||
         match(p, T_EQ) || match(p, T_NE)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_range(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* ewand : reln ewand_next ;
 *
 * ewand_next
 *  : T_ELEM_AND ewand
 *  | %empty
 *  ;
 */
PARSE_RULE (ewand)
  node = parse_reln(p);
  if (!node)
    return NULL;

  while (match(p, T_ELEM_AND)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_reln(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* ewor : ewand ewor_next ;
 *
 * ewor_next
 *  : T_ELEM_OR ewor
 *  | %empty
 *  ;
 */
PARSE_RULE (ewor)
  node = parse_ewand(p);
  if (!node)
    return NULL;

  while (match(p, T_ELEM_OR)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_ewand(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* lgand : ewor lgand_next ;
 *
 * lgand_next
 *  : T_AND lgand
 *  | %empty
 *  ;
 */
PARSE_RULE (lgand)
  node = parse_ewor(p);
  if (!node)
    return NULL;

  while (match(p, T_AND)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_ewor(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* lgor : lgand lgor_next ;
 *
 * lgor_next
 *  : T_OR lgor
 *  | %empty
 *  ;
 */
PARSE_RULE (lgor)
  node = parse_lgand(p);
  if (!node)
    return NULL;

  while (match(p, T_OR)) {
    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_lgand(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }

  return node;
}

/* === */

/* expr : lgor expr_next ;
 *
 * expr_next
 *  : T_ASSIGN expr
 *  | T_EQ_PLUS expr
 *  | T_EQ_MINUS expr
 *  | T_EQ_MUL expr
 *  | T_EQ_DIV expr
 *  | T_EQ_LDIV expr
 *  | T_EQ_POW expr
 *  | %empty
 *  ;
 *
 * this rule is right associative.
 */
PARSE_RULE (expr)
  node = parse_lgor(p);
  if (!node)
    return NULL;

  if (match(p, T_ASSIGN)) {
    if (ast_get_type(node) != (ASTNodeType) T_IDENT &&
        ast_get_type(node) != AST_TYPE_COLUMN)
      PARSE_ERR_LVALUE;

    if (!valid_lvalue(node))
      PARSE_ERR_LVALUE;

    node = ast_new_with_data(p, node);
    ast_add_down(node, parse_expr(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");
  }
  else if (match(p, T_EQ_PLUS) ||
           match(p, T_EQ_MINUS) ||
           match(p, T_EQ_MUL) ||
           match(p, T_EQ_DIV) ||
           match(p, T_EQ_LDIV) ||
           match(p, T_EQ_POW)) {
    if (ast_get_type(node) != (ASTNodeType) T_IDENT)
      PARSE_ERR_LVALUE;

    if (!valid_lvalue(node))
      PARSE_ERR_LVALUE;

    ScannerToken tbin = T_ERR;
    switch (p->tok) {
      case T_EQ_PLUS:  tbin = T_PLUS;  break;
      case T_EQ_MINUS: tbin = T_MINUS; break;
      case T_EQ_MUL:   tbin = T_MUL;   break;
      case T_EQ_DIV:   tbin = T_DIV;   break;
      case T_EQ_LDIV:  tbin = T_LDIV;  break;
      case T_EQ_POW:   tbin = T_POW;   break;
      default: break;
    }

    AST lhs = ast_copy(node);
    lhs = ast_new_with_parms(T_ASSIGN, false, lhs);
    ast_set_context(p, lhs);

    node = ast_new_with_parms(tbin, false, node);
    ast_set_context(p, node);
    next(p);

    ast_add_down(node, parse_expr(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");

    ast_add_down(lhs, node);
    node = lhs;
  }

  return node;
}

/* === */

/* ids : T_IDENT ids_next ;
 *
 * ids_next
 *  : T_IDENT ids
 *  | %empty
 *  ;
 */
int parse_ids (Parser p, AST node) {
  int n = 0;

  while (match(p, T_IDENT)) {
    ast_add_down(node, ast_new_with_data(p, NULL));
    n++;
  }

  return n;
}

/* persist : T_PERSISTENT ids ;
 */
PARSE_RULE (persist)
  if (!accept(p, T_PERSISTENT))
    return NULL;

  node = ast_new_with_type(T_PERSISTENT);
  if (!parse_ids(p, node))
    PARSE_ERR_MISSING("identifier(s)");

  return node;
}

/* global : T_GLOBAL ids ;
 */
PARSE_RULE (global)
  if (!accept(p, T_GLOBAL))
    return NULL;

  node = ast_new_with_type(T_GLOBAL);
  if (!parse_ids(p, node))
    PARSE_ERR_MISSING("identifier(s)");

  return node;
}

/* === */

/* try : T_TRY stmt_end stmts T_CATCH catch_id stmt_end stmts T_END ;
 * catch_id : T_IDENT | %empty ;
 */
PARSE_RULE (try)
  if (!accept(p, T_TRY))
    return NULL;

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_type(T_TRY);
  ast_add_down(node, parse_stmts(p));

  PARSE_REQUIRE(T_CATCH);
  if (!match(p, T_IDENT))
    PARSE_ERR_MISSING("catch variable");

  ast_add_down(node, ast_new_with_data(p, NULL));

  PARSE_REQUIRE_STMT_END;
  ast_add_down(node, parse_stmts(p));
  PARSE_REQUIRE(T_END);

  return node;
}

/* === */

/* if : T_IF expr stmt_end stmts elifs else T_END ;
 *
 * elifs : elif elifs_next ;
 *
 * elifs_next
 *  : elif elifs_next
 *  | %empty
 *  ;
 *
 * elif : T_ELSEIF expr stmt_end stmts ;
 * else : T_ELSE stmt_end stmts ;
 */
PARSE_RULE (if)
  if (!accept(p, T_IF))
    return NULL;

  node = parse_expr(p);
  if (!node)
    PARSE_ERR_MISSING("expression");

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_parms(T_IF, false, node);
  ast_add_down(node, parse_stmts(p));

  while (accept(p, T_ELSEIF)) {
    ast_add_down(node, parse_expr(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("expression");

    PARSE_REQUIRE_STMT_END;
    ast_add_down(node, parse_stmts(p));
  }

  if (accept(p, T_ELSE)) {
    PARSE_REQUIRE_STMT_END;
    ast_add_down(node, NULL);
    ast_add_down(node, parse_stmts(p));
  }

  PARSE_REQUIRE(T_END);

  return node;
}

/* === */

/* switch : T_SWITCH expr stmt_end cases T_END ;
 *
 * cases
 *  : T_CASE value stmt_end stmts cases
 *  | T_OTHERWISE stmt_end stmts
 *  | %empty
 *  ;
 */
PARSE_RULE (switch)
  if (!accept(p, T_SWITCH))
    return NULL;

  node = parse_expr(p);
  if (!node)
    PARSE_ERR_MISSING("expression");

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_parms(T_SWITCH, false, node);

  while (accept(p, T_CASE)) {
    ast_add_down(node, parse_value(p));
    if (!ast_last(node))
      PARSE_ERR_MISSING("case value");

    PARSE_REQUIRE_STMT_END;
    ast_add_down(node, parse_stmts(p));
  }

  if (accept(p, T_OTHERWISE)) {
    PARSE_REQUIRE_STMT_END;
    ast_add_down(node, NULL);
    ast_add_down(node, parse_stmts(p));
  }

  PARSE_REQUIRE(T_END);

  return node;
}

/* === */

/* for : T_FOR T_IDENT T_ASSIGN lgor stmt_end stmts T_END ;
 */
PARSE_RULE (for)
  if (!accept(p, T_FOR))
    return NULL;

  if (!match(p, T_IDENT))
    PARSE_ERR_MISSING_TOKEN(T_IDENT);

  node = ast_new_with_type(T_FOR);
  ast_add_down(node, ast_new_with_data(p, NULL));

  PARSE_REQUIRE(T_ASSIGN);
  ast_add_down(node, parse_lgor(p));
  if (!ast_last(node))
    PARSE_ERR_MISSING("iterator expression");

  PARSE_REQUIRE_STMT_END;
  ast_add_down(node, parse_stmts(p));
  PARSE_REQUIRE(T_END);

  return node;
}

/* === */

/* while : T_WHILE expr stmt_end stmts T_END ;
 */
PARSE_RULE (while)
  if (!accept(p, T_WHILE))
    return NULL;

  node = parse_expr(p);
  if (!node)
    PARSE_ERR_MISSING("expression");

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_parms(T_WHILE, false, node);
  ast_add_down(node, parse_stmts(p));
  PARSE_REQUIRE(T_END);

  return node;
}

/* === */

/* until : T_DO stmt_end stmts T_UNTIL expr ;
 */
PARSE_RULE (until)
  if (!accept(p, T_DO))
    return NULL;

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_parms(T_UNTIL, false, parse_stmts(p));
  PARSE_REQUIRE(T_UNTIL);

  ast_add_down(node, parse_expr(p));
  if (!ast_last(node))
    PARSE_ERR_MISSING("expression");

  return node;
}

/* === */

/* stmt : stmt_body stmt_end ;
 *
 * stmt_body
 *  : T_BREAK
 *  | T_CONTINUE
 *  | T_RETURN
 *  | persist
 *  | global
 *  | try
 *  | if
 *  | switch
 *  | for
 *  | while
 *  | until
 *  | expr
 *  ;
 *
 * stmt_end
 *  : T_SEMI opt_eol
 *  | T_COMMA opt_eol
 *  | T_EOL
 *  ;
 *
 * opt_eol : T_EOL | %empty ;
 */
PARSE_RULE (stmt)
  PARSE_NEWLINES;

  if (match(p, T_BREAK) || match(p, T_CONTINUE) || match(p, T_RETURN))
    node = ast_new_with_data(p, NULL);
  else if (match(p, T_PERSISTENT))
    node = parse_persist(p);
  else if (match(p, T_GLOBAL))
    node = parse_global(p);
  else if (match(p, T_TRY))
    node = parse_try(p);
  else if (match(p, T_IF))
    node = parse_if(p);
  else if (match(p, T_SWITCH))
    node = parse_switch(p);
  else if (match(p, T_FOR))
    node = parse_for(p);
  else if (match(p, T_WHILE))
    node = parse_while(p);
  else if (match(p, T_DO))
    node = parse_until(p);
  else
    node = parse_expr(p);

  if (!node)
    return NULL;

  if (accept(p, T_SEMI))
    ast_set_disp(node, false);
  else if (accept(p, T_COMMA) || accept(p, T_EOL))
    ast_set_disp(node, true);
  else
    PARSE_ERR_MISSING("end of statement");

  while (match(p, T_SEMI) || match(p, T_COMMA) || match(p, T_EOL))
    next(p);

  return node;
}

/* stmts : stmt stmts_next ;
 * stmts_next : stmt | %empty ;
 */
PARSE_RULE (stmts)
  node = parse_stmt(p);
  if (!node)
    return NULL;

  AST down = parse_stmt(p);
  while (down) {
    if (ast_get_type(node) != AST_TYPE_STATEMENTS)
      node = ast_new_with_parms(AST_TYPE_STATEMENTS, false, node);

    ast_add_down(node, down);
    down = parse_stmt(p);
  }

  return node;
}

/* === */

/* args : T_IDENT args_next ;
 *
 * args_next
 *  : T_COMMA args
 *  | %empty
 *  ;
 */
PARSE_RULE (args)
  if (!match(p, T_IDENT))
    return NULL;

  node = ast_new_with_data(p, NULL);
  node = ast_new_with_parms(AST_TYPE_IDS, false, node);

  while (accept(p, T_COMMA)) {
    if (!match(p, T_IDENT))
      PARSE_ERR_MISSING_TOKEN(T_IDENT);

    ast_add_down(node, ast_new_with_data(p, NULL));
  }

  return node;
}

/* function : T_FUNCTION function_core argin stmt_end stmts T_END ;
 *
 * function_core
 *  : T_IDENT function_name
 *  | argout T_ASSIGN T_IDENT
 *  ;
 *
 * function_name
 *  : T_ASSIGN T_IDENT
 *  | %empty
 *  ;
 *
 * argin
 *  : T_PAREN_OPEN args T_PAREN_CLOSE
 *  | %empty
 *  ;
 *
 * argout : T_BRACK_OPEN args T_BRACK_CLOSE ;
 */
PARSE_RULE (function)
  AST argout = NULL;

  if (!accept(p, T_FUNCTION))
    return NULL;

  node = ast_new_with_type(AST_TYPE_FUNCTION);
  if (match(p, T_IDENT)) {
    argout = ast_new_with_data(p, NULL);

    if (accept(p, T_ASSIGN)) {
      ast_add_down(node, argout);
      if (!match(p, T_IDENT))
        PARSE_ERR_MISSING("function name");

      ast_add_down(node, ast_new_with_data(p, NULL));
    }
    else {
      ast_add_down(node, NULL);
      ast_add_down(node, argout);
    }
  }
  else if (accept(p, T_BRACK_OPEN)) {
    argout = parse_args(p);
    ast_add_down(node, argout);
    PARSE_REQUIRE(T_BRACK_CLOSE);
    PARSE_REQUIRE(T_ASSIGN);

    if (!match(p, T_IDENT))
      PARSE_ERR_MISSING("function name");

    ast_add_down(node, ast_new_with_data(p, NULL));
  }
  else PARSE_ERR;

  if (accept(p, T_PAREN_OPEN)) {
    ast_add_down(node, parse_args(p));
    PARSE_REQUIRE(T_PAREN_CLOSE);
  }
  else
    ast_add_down(node, NULL);

  PARSE_REQUIRE_STMT_END;
  ast_add_down(node, parse_stmts(p));
  PARSE_REQUIRE(T_END);

  return node;
}

/* === */

/* properties : T_PROPERTIES stmt_end proplist T_END | %empty ;
 * proplist : prop proplist | %empty ;
 *
 * prop : T_IDENT prop_default stmt_end ;
 * prop_default : T_ASSIGN value | %empty ;
 */
PARSE_RULE (properties)
  if (!accept(p, T_PROPERTIES))
    return NULL;

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_type(T_PROPERTIES);

  while (match(p, T_IDENT)) {
    ast_add_down(node, ast_new_with_data(p, NULL));

    if (accept(p, T_ASSIGN)) {
      ast_add_down(node, parse_value(p));
      if (!ast_last(node))
        PARSE_ERR_MISSING("property value");
    }
    else
      ast_add_down(node, NULL);

    PARSE_REQUIRE_STMT_END;
  }

  PARSE_REQUIRE(T_END);
  return node;
}

/* -- */

/* methods : T_METHODS stmt_end method_list T_END | %empty ;
 * method_list : function stmt_end method_list | %empty ;
 */
PARSE_RULE (methods)
  if (!accept(p, T_METHODS))
    return NULL;

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_type(T_METHODS);

  do {
    PARSE_NEWLINES;
    ast_add_down(node, parse_function(p));
    PARSE_NEWLINES;
  } while (ast_last(node));

  ast_shrink_down(node);
  PARSE_REQUIRE(T_END);
  return node;
}

/* -- */

/* events : T_EVENTS stmt_end evlist T_END | %empty ;
 * evlist : event evlist | %empty ;
 * event : T_IDENT stmt_end ;
 */
PARSE_RULE (events)
  if (!accept(p, T_EVENTS))
    return NULL;

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_type(T_EVENTS);

  while (match(p, T_IDENT)) {
    ast_add_down(node, ast_new_with_data(p, NULL));
    PARSE_REQUIRE_STMT_END;
  }

  PARSE_REQUIRE(T_END);
  return node;
}

/* -- */

/* enums : T_ENUM stmt_end enumlist T_END | %empty ;
 * enumlist : enum enumlist | %empty ;
 * enum : T_IDENT enum_end stmt_end ;
 * enum_end : T_PAREN_OPEN value T_PAREN_CLOSE | %empty ;
 */
PARSE_RULE (enums)
  if (!accept(p, T_ENUM))
    return NULL;

  PARSE_REQUIRE_STMT_END;
  node = ast_new_with_type(T_ENUM);

  while (match(p, T_IDENT)) {
    ast_add_down(node, ast_new_with_data(p, NULL));

    if (accept(p, T_PAREN_OPEN)) {
      ast_add_down(node, parse_value(p));
      PARSE_REQUIRE(T_PAREN_CLOSE);
    }
    else
      ast_add_down(node, NULL);

    PARSE_REQUIRE_STMT_END;
  }

  PARSE_REQUIRE(T_END);
  return node;
}

/* -- */

/* inherits : T_LT T_IDENT inherits_next stmt_end ;
 * inherits_next : T_ELEM_AND T_IDENT inherits_next | %empty ;
 */
PARSE_RULE (inherits)
  if (!accept(p, T_LT))
    return NULL;

  node = ast_new_with_type(AST_TYPE_IDS);
  if (!match(p, T_IDENT))
    PARSE_ERR_MISSING_TOKEN(T_IDENT);

  ast_add_down(node, ast_new_with_data(p, NULL));

  while (accept(p, T_ELEM_AND)) {
    if (!match(p, T_IDENT))
      PARSE_ERR_MISSING_TOKEN(T_IDENT);

    ast_add_down(node, ast_new_with_data(p, NULL));
  }

  return node;
}

/* class : T_CLASSDEF T_IDENT inherits props methods events enums T_END ;
 */
PARSE_RULE (class)
  AST down;

  bool have_props, have_methods, have_events, have_enums;
  have_props = have_methods = have_events = have_enums = false;

  if (!accept(p, T_CLASSDEF))
    return NULL;

  if (!match(p, T_IDENT))
    PARSE_ERR_MISSING_TOKEN(T_IDENT);

  node = ast_new_with_data(p, NULL);
  node = ast_new_with_parms(AST_TYPE_CLASS, false, node);
  ast_add_down(node, parse_inherits(p));

  PARSE_REQUIRE_STMT_END;

  while (1) {
    PARSE_NEWLINES;

    if (match(p, T_PROPERTIES)) {
      if (have_props)
        PARSE_ERR_EXISTS(p->tok);

      ast_add_down(node, parse_properties(p));
      have_props = true;
    }
    else if (match(p, T_METHODS)) {
      if (have_methods)
        PARSE_ERR_EXISTS(p->tok);

      ast_add_down(node, parse_methods(p));
      have_methods = true;
    }
    else if (match(p, T_EVENTS)) {
      if (have_events)
        PARSE_ERR_EXISTS(p->tok);

      ast_add_down(node, parse_events(p));
      have_events = true;
    }
    else if (match(p, T_ENUM)) {
      if (have_enums)
        PARSE_ERR_EXISTS(p->tok);

      ast_add_down(node, parse_enums(p));
      have_enums = true;
    }
    else
      break;
  }

  PARSE_REQUIRE(T_END);
  return node;
}

/* === */

/* block : class | function | stmt ;
 */
PARSE_RULE (block)
  PARSE_NEWLINES;

  if (match(p, T_CLASSDEF))
    node = parse_class(p);
  else if (match(p, T_FUNCTION))
    node = parse_function(p);
  else
    node = parse_stmt(p);

  return node;
}

/* blocks : block blocks_next ;
 * blocks_next : block | %empty ;
 */
PARSE_RULE (blocks)
  node = parse_block(p);
  if (!node)
    return NULL;

  AST down = parse_block(p);
  while (down) {
    if (ast_get_type(node) != AST_TYPE_ROOT)
      node = ast_new_with_parms(AST_TYPE_ROOT, false, node);

    ast_add_down(node, down);
    down = parse_block(p);
  }

  return node;
}

/* === */

/* parse(): execute a complete parse of the currently set file
 * or string buffered scanner, producing an internal abstract
 * syntax tree as a result.
 *
 * arguments:
 *  @p: matte parser to access.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
static int parse (Parser p) {
  /* force initialization of the parsing token. */
  p->tok = T_ERR;

  /* construct an abstract syntax tree. */
  AST subtree = parse_blocks(p);

  /* check for errors. */
  if (p->err || p->scan->err) {
    /* combine the parser and scanner errors. */
    p->err += p->scan->err;

    /* output a final error message. */
    fprintf(stderr, "%s: there were errors. cannot continue.\n",
            scanner_get_filename(p->scan));

    /* free the abstract syntax tree. */
    object_free((Object) subtree);

    /* return failure. */
    return 0;
  }

  /* combine the newly parsed subtree into the existing tree. */
  p->tree = ast_merge(p->tree, subtree);

  /* return success. */
  return 1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* parser_type(): return a pointer to the parser object type.
 */
ObjectType parser_type (void) {
  /* return the struct pointer. */
  return &Parser_type;
}

/* parser_new(): allocate a new matte parser.
 *
 * returns:
 *  newly allocated and initialized matte parser.
 */
Parser parser_new (Object args) {
  /* allocate a new parser. */
  Parser p = (Parser) Parser_type.fn_alloc(&Parser_type);
  if (!p)
    return NULL;

  /* allocate the scanner. */
  p->scan = scanner_new(NULL);
  p->tok = T_ERR;

  /* check if scanner allocation failed. */
  if (!p->scan)
    return NULL;

  /* initialize the syntax tree. */
  p->tree = NULL;
  p->end_valid = false;

  /* initialize the error count. */
  p->err = 0L;

  /* return the new parser. */
  return p;
}

/* parser_new_with_file(): allocate a new matte parser associated with
 * a filename string.
 *
 * arguments:
 *  @fname: input filename string.
 *
 * returns:
 *  newly allocated and initialized parser.
 */
Parser parser_new_with_file (const char *fname) {
  /* declare required variables:
   *  @p: new parser to allocate.
   */
  Parser p;

  /* allocate a new parser and set the filename string. */
  p = parser_new(NULL);
  if (!p || !parser_set_file(p, fname)) {
    object_free((Object) p);
    return NULL;
  }

  /* return the new parser. */
  return p;
}

/* parser_new_with_string(): allocate a new matte parser associated with
 * a buffer string.
 *
 * arguments:
 *  @str: input buffer string.
 *
 * returns:
 *  newly allocated and initialized parser.
 */
Parser parser_new_with_string (const char *str) {
  /* declare required variables:
   *  @p: new parser to allocate.
   */
  Parser p;

  /* allocate a new parser and set the string buffer. */
  p = parser_new(NULL);
  if (!p || !parser_set_string(p, str)) {
    object_free((Object) p);
    return NULL;
  }

  /* return the new parser. */
  return p;
}

/* parser_free(): free all memory associated with a matte parser.
 *
 * arguments:
 *  @p: matte parser to free.
 */
void parser_free (Parser p) {
  /* return if the parser is null. */
  if (!p)
    return;

  /* free the associated scanner. */
  object_free((Object) p->scan);

  /* free the abstract syntax tree. */
  object_free((Object) p->tree);
}

/* parser_set_file(): set the filename string of a matte parser.
 *
 * arguments:
 *  @p: matte parser to modify.
 *  @fname: new filename string to set.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int parser_set_file (Parser p, const char *fname) {
  /* validate the input arguments. */
  if (!p || !fname)
    fail("invalid input arguments");

  /* attempt to set the file in the associated scanner. */
  if (!scanner_set_file(p->scan, fname))
    return 0;

  /* execute the parse operation. */
  return parse(p);
}

/* parser_set_string(): set the buffer string of a matte parser.
 *
 * arguments:
 *  @p: matte parser to modify.
 *  @str: new buffer string to set.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int parser_set_string (Parser p, const char *str) {
  /* validate the input arguments. */
  if (!p || !str)
    fail("invalid input arguments");

  /* attempt to set the string in the associated scanner. */
  if (!scanner_set_string(p->scan, str))
    return 0;

  /* execute the parse operation. */
  return parse(p);
}

/* ast_new_with_data(): allocate a new matte abstract syntax tree node
 * based on the current state of a matte parser.
 *
 * arguments:
 *  @p: matte parser to access.
 *  @down: downstream node, or NULL for none.
 *
 * returns:
 *  newly allocated and initialized matte ast-node.
 */
AST ast_new_with_data (Parser p, AST down) {
  /* declare required variables:
   *  @node: new matte ast-node.
   *  @str: string value from scanner.
   */
  AST node;
  char *str;

  /* allocate a new ast-node. */
  node = ast_new_with_type(p->tok);
  if (!node)
    return NULL;

  /* set the context and display flag. */
  ast_set_context(p, node);
  ast_set_disp(node, false);

  /* add the downstream node, if requested. */
  if (down)
    ast_add_down(node, down);

  /* determine whether we can store any simple data in the node. */
  switch (p->tok) {
    /* identifiers: directly set the string data. */
    case T_IDENT:
      str = scanner_get_string(p->scan);
      ast_set_string(node, str);
      free(str);
      break;

    /* strings: set and modify the string data. */
    case T_STRING:
      str = scanner_get_string(p->scan);
      str[0] = str[strlen(str) - 1] = '"';
      ast_set_string(node, str);
      free(str);
      break;

    /* integers: set the int data. */
    case T_INT:
      ast_set_int(node, scanner_get_int(p->scan));
      break;

    /* floats: set the float data. */
    case T_FLOAT:
      ast_set_float(node, scanner_get_float(p->scan));
      break;

    /* complex floats: set the complex data. */
    case T_COMPLEX:
      ast_set_complex(node, scanner_get_complex(p->scan));
      break;

    /* all other tokens: no data. */
    default:
      break;
  }

  /* read a new token of lookahead into the parser. */
  next(p);

  /* return the new tree node. */
  return node;
}

/* Parser_type: object type structure for matte parsers.
 */
struct _ObjectType Parser_type = {
  "Parser",                                      /* name       */
  sizeof(struct _Parser),                        /* size       */
  0,                                             /* precedence */

  (obj_constructor) parser_new,                  /* fn_new     */
  NULL,                                          /* fn_copy    */
  (obj_allocator)   object_alloc,                /* fn_alloc   */
  (obj_destructor)  parser_free,                 /* fn_dealloc */
  NULL,                                          /* fn_disp    */

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

