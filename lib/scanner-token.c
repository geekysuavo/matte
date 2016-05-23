
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the scanner header. */
#include <matte/scanner.h>

/* map: array of token name strings mapped to their enumeration values.
 */
static const struct {
  /* @tok: token enumeration value.
   * @name: token string representation.
   */
  ScannerToken tok;
  const char *name;
} 
map[] = {
  { T_ERR, "error"        },
  { T_EOF, "end of input" },
  { T_EOL, "newline"      },

  { T_BRACK_OPEN,  "open bracket"     },
  { T_BRACE_OPEN,  "open brace"       },
  { T_PAREN_OPEN,  "open parenthesis" },
  { T_BRACK_CLOSE, "end bracket"      },
  { T_BRACE_CLOSE, "end brace"        },
  { T_PAREN_CLOSE, "end parenthesis"  },

  { T_AS, "asterisk" },

  { T_POINT, "point"     },
  { T_COMMA, "comma"     },
  { T_SEMI,  "semicolon" },

  { T_INC, "++" },
  { T_DEC, "--" },

  { T_HTR, "'"  },
  { T_TR,  ".'" },

  { T_POW,      "^"  },
  { T_ELEM_POW, ".^" },

  { T_PLUS,  "+" },
  { T_MINUS, "-" },
  { T_NOT,   "!" },

  { T_MUL,       "*"   },
  { T_DIV,       "/"   },
  { T_LDIV,      "\\"  },
  { T_ELEM_MUL,  ".*"  },
  { T_ELEM_DIV,  "./"  },
  { T_ELEM_LDIV, ".\\" },

  { T_COLON, ":" },

  { T_LT, "<"  },
  { T_LE, "<=" },
  { T_EQ, "==" },
  { T_GE, ">=" },
  { T_GT, ">"  },
  { T_NE, "!=" },

  { T_ELEM_AND, "&" },
  { T_ELEM_OR,  "|" },

  { T_AND, "&&" },
  { T_OR,  "||" },

  { T_ASSIGN,   "="   },
  { T_EQ_PLUS,  "+="  },
  { T_EQ_MINUS, "-="  },
  { T_EQ_MUL,   "*="  },
  { T_EQ_DIV,   "/="  },
  { T_EQ_LDIV,  "\\=" },
  { T_EQ_POW,   "^="  },

  { T_BREAK,      "break"       },
  { T_CASE,       "case"        },
  { T_CATCH,      "catch"       },
  { T_CLASSDEF,   "classdef"    },
  { T_CONTINUE,   "continue"    },
  { T_DO,         "do"          },
  { T_ELSE,       "else"        },
  { T_ELSEIF,     "elseif"      },
  { T_END,        "end"         },
  { T_ENUM,       "enumeration" },
  { T_EVENTS,     "events"      },
  { T_FOR,        "for"         },
  { T_FUNCTION,   "function"    },
  { T_GLOBAL,     "global"      },
  { T_IF,         "if"          },
  { T_METHODS,    "methods"     },
  { T_OTHERWISE,  "otherwise"   },
  { T_PERSISTENT, "persistent"  },
  { T_PROPERTIES, "properties"  },
  { T_RETURN,     "return"      },
  { T_SWITCH,     "switch"      },
  { T_TRY,        "try"         },
  { T_UNTIL,      "until"       },
  { T_WHILE,      "while"       },

  { T_INT,     "int"        },
  { T_FLOAT,   "float"      },
  { T_COMPLEX, "complex"    },
  { T_STRING,  "string"     },
  { T_IDENT,   "identifier" },

  { T_ERR, NULL }
};

/* scanner_token_get_name(): get the string representation of a scanner
 * token enumeration value.
 *
 * arguments:
 *  @tok: token enumeration value.
 *
 * returns:
 *  string representation of the token, or "unknown" if the value is
 *  unrecognized.
 */
const char *scanner_token_get_name (ScannerToken tok) {
  /* declare required variables:
   *  @i: token mapping array index.
   */
  int i;

  /* loop over the token mapping. */
  for (i = 0; map[i].name; i++) {
    /* if the token value matches, return its string name. */
    if (map[i].tok == tok)
      return map[i].name;
  }

  /* return a default 'error' string. */
  return "unknown";
}

