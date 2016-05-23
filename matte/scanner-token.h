
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_SCANNER_TOKEN_H__
#define __MATTE_SCANNER_TOKEN_H__

/* ScannerToken: an enum _ScannerToken. */
typedef enum _ScannerToken ScannerToken;

/* _ScannerToken: enumeration that holds the token codes that may
 * be emitted from a matte scanner.
 */
enum _ScannerToken {
  /* unknown token and end of line. */
  T_ERR = 0,
  T_EOF,
  T_EOL,

  /* brackets, braces and parentheses. */
  T_BRACK_OPEN = 100,    /* "[" */
  T_BRACE_OPEN,          /* "{" */
  T_PAREN_OPEN,          /* "(" */
  T_BRACK_CLOSE,         /* "]" */
  T_BRACE_CLOSE,         /* "}" */
  T_PAREN_CLOSE,         /* ")" */

  /* superclass indexing. */
  T_AS = 200,            /* "@" */

  /* struct indexing, commas and semicolons. */
  T_POINT,               /* "." */
  T_COMMA,               /* "," */
  T_SEMI,                /* ";" */

  /* unary increment and decrement operators. */
  T_INC = 300,           /* "++" */
  T_DEC,                 /* "--" */

  /* unary transpose operators. */
  T_HTR,                 /* "'" */
  T_TR,                  /* ".'" */

  /* binary exponentiation operators. */
  T_POW = 400,           /* "^" */
  T_ELEM_POW,            /* ".^" */

  /* sign and logical negation operators. */
  T_PLUS,                /* "+", ".+" */
  T_MINUS,               /* "-", ".-" */
  T_NOT,                 /* "!", "~" */

  /* binary multiplication operators. */
  T_MUL,                 /* "*" */
  T_DIV,                 /* "/" */
  T_LDIV,                /* "\" */
  T_ELEM_MUL,            /* ".*" */
  T_ELEM_DIV,            /* "./" */
  T_ELEM_LDIV,           /* ".\" */

  /* indexing colon. */
  T_COLON,               /* ":" */

  /* numeric relational operators. */
  T_LT = 500,            /* "<" */
  T_LE,                  /* "<=" */
  T_EQ,                  /* "==" */
  T_GE,                  /* ">=" */
  T_GT,                  /* ">" */
  T_NE,                  /* "!=", "~=" */

  /* element-wise logical relational operators. */
  T_ELEM_AND = 600,      /* "&" */
  T_ELEM_OR,             /* "|" */

  /* logical relational operators. */
  T_AND,                 /* "&&" */
  T_OR,                  /* "||" */

  /* assignment. */
  T_ASSIGN = 700,        /* "=" */
  T_EQ_PLUS,             /* "+=" */
  T_EQ_MINUS,            /* "-=" */
  T_EQ_MUL,              /* "*=" */
  T_EQ_DIV,              /* "/=" */
  T_EQ_LDIV,             /* "\=" */
  T_EQ_POW,              /* "^=" */

  /* keywords. */
  T_BREAK = 800,         /* "break" */
  T_CASE,                /* "case" */
  T_CATCH,               /* "catch" */
  T_CLASSDEF,            /* "classdef" */
  T_CONTINUE,            /* "continue" */
  T_DO,                  /* "do" */
  T_ELSE,                /* "else" */
  T_ELSEIF,              /* "elseif" */
  T_END,                 /* "end" */
  T_ENUM,                /* "enumeration" */
  T_EVENTS,              /* "events" */
  T_FOR,                 /* "for" */
  T_FUNCTION,            /* "function" */
  T_GLOBAL,              /* "global" */
  T_IF,                  /* "if" */
  T_METHODS,             /* "methods" */
  T_OTHERWISE,           /* "otherwise" */
  T_PERSISTENT,          /* "persistent" */
  T_PROPERTIES,          /* "properties" */
  T_RETURN,              /* "return" */
  T_SWITCH,              /* "switch" */
  T_TRY,                 /* "try" */
  T_UNTIL,               /* "until" */
  T_WHILE,               /* "while" */

  /* literals. */
  T_INT = 900,      
  T_FLOAT,
  T_COMPLEX,
  T_STRING,

  /* identifiers. */
  T_IDENT
};

/* function declarations: */

const char *scanner_token_get_name (ScannerToken tok);

#endif /* !__MATTE_SCANNER_TOKEN_H__ */

