
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_PARSER_H__
#define __MATTE_PARSER_H__

/* include the object header. */
#include <matte/object.h>

/* include the matte scanner and token headers. */
#include <matte/scanner.h>
#include <matte/scanner-token.h>

/* inclue the abstract syntax tree header. */
#include <matte/ast.h>

/* IS_PARSER: macro to check that an object is a matte parser.
 */
#define IS_PARSER(obj) \
  MATTE_TYPE_CHECK(obj, parser_type())

/* Parser: pointer to a struct _Parser. */
typedef struct _Parser *Parser;
struct _ObjectType Parser_type;

/* Parser: structure for holding the current state of a matte parser.
 */
struct _Parser {
  /* base object. */
  OBJECT_BASE;

  /* @scan: matte scanner used for reading tokens from files and strings.
   * @tok: single required token of lookahead from the scanner.
   */
  Scanner scan;
  ScannerToken tok;

  /* @tree: abstract syntax tree used to store parsed files and strings.
   * @end_valid: whether or not 'end' tokens are valid as values.
   */
  AST tree;
  bool end_valid;

  /* @err: number of errors encountered by the parser.
   */
  int err;
};

/* function declarations (parser.c): */

ObjectType parser_type (void);

Parser parser_new (Object args);

Parser parser_new_with_file (const char *fname);

Parser parser_new_with_string (const char *str);

void parser_free (Parser p);

int parser_set_file (Parser p, const char *fname);

int parser_set_string (Parser p, const char *str);

AST ast_new_with_data (Parser p);

#endif /* !__MATTE_PARSER_H__ */

