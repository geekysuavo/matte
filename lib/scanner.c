
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the scanner header. */
#include <matte/scanner.h>

/* SCANNER_BUF_SIZE: total scanner buffer size.
 * SCANNER_BUF_MARGIN: buffer padding to maintain.
 */
#define SCANNER_BUF_SIZE    4096L
#define SCANNER_BUF_MARGIN    16L

/* scanerr(): macro function to print an immediate scanner error
 * message. errorfn() handles un-termination.
 */
#define scanerr(s, ...) \
 { term(s); errorfn(s, __VA_ARGS__); }

/* reserved: array of reserved keyword lexemes that may not be
 * interpreted as identifiers.
 */
static const struct {
  /* @tok: token of the reserved word.
   * @lex: lexeme of the reserved word.
   */
  ScannerToken tok;
  const char *lex;
} 
reserved[] = {
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
  { T_ERR,        NULL          }
};

/* char_is_whitespace(): check if a character matches a whitespace pattern.
 *
 * arguments:
 *  @c: character to check.
 *
 * returns:
 *  integer indicating whether (1) or not (0) the character is whitespace.
 */
static inline int char_is_whitespace (char c) {
  /* check the character. */
  return (c == ' ' || c == '\t');
}

/* char_is_digit(): check if a character matches the [0-9] expression.
 *
 * arguments:
 *  @c: character to check.
 *
 * returns:
 *  integer indicating whether (1) or not (0) the character is a digit.
 */
static inline int char_is_digit (char c) {
  /* check the character. */
  return (c >= '0' && c <= '9');
}

/* char_is_ident(): check if a character matches the [a-zA-Z0-9_] expression.
 *
 * arguments:
 *  @c: character to check.
 *
 * returns:
 *  integer indicating whether (1) or not (0) the character is a valid
 *  identifier character.
 */
static inline int char_is_ident (char c) {
  /* check the character. */
  return ((c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') ||
           c == '_');
}

/* term(): temporarily null-terminate the current token of a scanner.
 *
 * this action must be undone by scanner_unterm() prior to continued
 * scanning.
 *
 * arguments:
 *  @s: matte scanner to access.
 */
static inline void term (Scanner s) {
  /* store the token end character and null-terminate the token. */
  s->term = *s->tok_end;
  *s->tok_end = '\0';
}

/* unterm(): undo temporary null-termination of the current token
 * of a scanner.
 *
 * arguments:
 *  @s: matte scanner to access.
 */
static inline void unterm (Scanner s) {
  /* restore the stored token end character. */
  *s->tok_end = s->term;
}

/* errorfn(): print an immediate scanner error message and
 * increment the error count of a scanner.
 *
 * this is the core function used by the scanner_error() macro function,
 * which basically just terminates the scanner's token, calls this function,
 * and unterminates the token.
 *
 * arguments:
 *  @s: matte scanner to access and modify.
 *  @format: printf-style error message format string.
 *  @...: arguments corresponding to @format.
 */
static void errorfn (Scanner s, const char *format, ...) {
  /* declare required variables:
   *  @vl: variable-length arguments list for message printing.
   *  @line: current line string.
   */
  va_list vl;
  char *line;

  /* flush standard output. */
  fflush(stdout);

  /* print the initial portion of the error message. */
  fprintf(stderr, "%s:%ld: error: ", s->fname ? s->fname : "(string)",
          s->lineno);

  /* print the custom error message and a newline. */
  va_start(vl, format);
  vfprintf(stderr, format, vl);
  fprintf(stderr, "\n");
  fflush(stderr);
  va_end(vl);

  /* construct and print the line string. */
  unterm(s);
  line = scanner_get_linestr(s);
  if (line) {
    fprintf(stderr, "%s", line);
    free(line);
  }

  /* increment the scanner error count. */
  s->err++;
}

/* lookahead(): get the current lookahead character from the scanner.
 *
 * for scanners that are buffering their input from files, this function
 * will also manage buffer flush operations. buffer flushes are triggered
 * when the end of a token becomes too close to the end of the buffer.
 *
 * arguments:
 *  @s: matte scanner to access.
 *  @n: number of characters to look ahead.
 *
 * returns:
 *  lookahead character in the scanner buffer, or '\0' on end-of-stream
 *  or buffer flush/expand failure.
 */
static char lookahead (Scanner s, int n) {
  /* declare required variables:
   *  @nread: number of characters read during a buffer flush.
   *  @nread_max: target number of characters to read.
   *  @p, @pbuf: pointers into the buffer.
   */
  long nread, nread_max;
  char *p, *pbuf;

  /* determine the lookahead pointer. */
  p = s->tok_end + (n - 1);

  /* check if the pointer is out of bounds. */
  if (p < s->buf || p > s->buf_end)
    return '\0';

  /* tentatively accept the lookahead as part of the current token. */
  if (p >= s->tok_end)
    s->tok_end = p + 1;

  /* check if the buffer needs to be flushed or expanded. */
  if (s->fd >= 0 && s->buf_end - s->tok_end < SCANNER_BUF_MARGIN) {
    /* shift the remaining data to the start of the buffer. */
    p = s->tok;
    pbuf = s->buf;
    while (p < s->buf_end) {
      s->buf_pos++;
      *pbuf = *p;
      pbuf++;
      p++;
    }

    /* update the buffer size and end pointer. */
    s->buf_size = s->buf_end - s->tok;
    s->buf_end = s->buf + s->buf_size;

    /* compute the new token end pointer. */
    p = s->tok;
    pbuf = s->buf;
    while (p < s->tok_end) {
      pbuf++;
      p++;
    }

    /* update the token start and end pointers. */
    s->tok = s->buf;
    s->tok_end = pbuf;

    /* read new data into the end of the buffer. */
    nread_max = SCANNER_BUF_SIZE - s->buf_size;
    nread = read(s->fd, s->buf + s->buf_size, nread_max);

    /* update the buffer size and end pointer. */
    s->buf_size += nread;
    s->buf_end = s->buf + s->buf_size;

    /* check if the file contents were exhausted. */
    if (nread < nread_max) {
      /* close the input file. */
      close(s->fd);
      s->fd = -1;
    }
  }

  /* return the character. */
  return *p;
}

/* pushback(): push lookahead characters back to the scanner buffer.
 *
 * arguments:
 *  @s: matte scanner to access.
 *  @n: number of characters to push back.
 */
static void pushback (Scanner s, int n) {
  /* declare required variables:
   *  @p: pointer into the scanner buffer.
   */
  char *p;

  /* determine the pushback pointer. */
  p = s->tok_end - n;

  /* do nothing if the pointer is out of bounds. */
  if (p < s->buf || p > s->buf_end)
    return;

  /* if the pushback pointer is to the right of the token start pointer,
   * then set the token end to the pushback. otherwise, reset the token.
   */
  if (p > s->tok)
    s->tok_end = p;
  else
    s->tok_end = s->tok + 1;
}

/* current(): get the current token end character without looking ahead.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  last token character in the scanner buffer.
 */
static inline char current (Scanner s) {
  /* return the last character of the current token. */
  return *(s->tok_end - 1);
}

/* state_str(): scanner state handling lexemes matching the string pattern.
 * accepted strings must be defined on a single line.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *  @ter: string terminator, either a single or double quote.
 *
 * returns:
 *  on successful string parsing, returns T_STRING.
 */
static ScannerToken state_str (Scanner s, char ter) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* loop until an end of stream is reached. */
  while (look) {
    /* attempt to locate terminating characters. */
    if (look == ter)
      return T_STRING;
    else if (look == '\n')
      break;

    /* read another character of lookahead. */
    look = lookahead(s, 1);
  }

  /* return an error. */
  pushback(s, 1);
  scanerr(s, "malformed string %s", s->tok);
  return T_ERR;
}

/* state_float_end(): scanner state handling the terminal characters
 * of floats in scientific notation.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  on success, T_FLOAT. on failure, T_ERR.
 */
static ScannerToken state_float_end (Scanner s) {
  /* declare required variables:
   *  @cur: current token end character.
   *  @look: lookahead character.
   */
  char cur, look;

  /* read the token end character and a one character of lookahead. */
  cur = current(s);
  look = lookahead(s, 1);

  /* accept digits to terminate the lexeme. */
  if (char_is_digit(look))
    return state_float_end(s);

  /* allow terminal [ijIJ] to coerce floats to complex floats. */
  switch (look) {
    case 'i':
    case 'j':
    case 'I':
    case 'J': return T_COMPLEX;
  }

  /* push back the lookahead character. */
  pushback(s, 1);

  /* if no digits, return an error. */
  if (cur == '-' || cur == '+') {
    scanerr(s, "malformed float '%s'", s->tok);
    return T_ERR;
  }

  /* if nothing else, return a float. */
  return T_FLOAT;
}

/* state_float_exp(): scanner state handling the transition between
 * vanilla floats and floats in scientific notation.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  on success, T_FLOAT. or T_ERR on failure.
 */
static ScannerToken state_float_exp (Scanner s) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* accept signed digits to move into the terminating state. */
  if (look == '-' || look == '+' || char_is_digit(look))
    return state_float_end(s);

  /* if no digits, return an error. */
  pushback(s, 1);
  scanerr(s, "malformed float '%s'", s->tok);
  return T_ERR;
}

/* state_float(): scanner state handling lexemes that currently
 * match the float pattern.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  on success, T_FLOAT.
 */
static ScannerToken state_float (Scanner s) {
  /* declare required variables:
   *  @cur: current token end character.
   *  @look: lookahead character.
   */
  char cur, look;

  /* read the token end character and a one character of lookahead. */
  cur = current(s);
  look = lookahead(s, 1);

  /* attempt to expand the lexeme. */
  switch (look) {
    /* scientific notation may only begin after digits. */
    case 'e':
    case 'E':
      /* if something like '1.e', return an error. */
      if (cur == '.') {
        scanerr(s, "malformed float '%s'", s->tok);
        return T_ERR;
      }

      /* move into the scientific notation float state. */
      return state_float_exp(s);

    /* terminating [ijIJ] coerces floats to complex floats. */
    case 'i':
    case 'j':
    case 'I':
    case 'J': return T_COMPLEX;
  }

  /* check if additional digits are available. */
  if (char_is_digit(look))
    return state_float(s);

  /* if nothing else, push back a character and return a float. */
  pushback(s, 1);
  return T_FLOAT;
}

/* state_int(): scanner state handling lexemes that currently
 * match the integer pattern.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  any of T_INT or T_FLOAT.
 */
static ScannerToken state_int (Scanner s) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* attempt to expand the lexeme. */
  switch (look) {
    /* points coerce ints to floats. */
    case '.': return state_float(s);

    /* scientific notation coerces ints to floats. */
    case 'e':
    case 'E': return state_float_exp(s);

    /* terminating [ijIJ] coerces ints to complex floats. */
    case 'i':
    case 'j':
    case 'I':
    case 'J': return T_COMPLEX;
  }

  /* check if additional digits are available. */
  if (char_is_digit(look))
    return state_int(s);

  /* if nothing else, push back a character and return an integer. */
  pushback(s, 1);
  return T_INT;
}

/* state_point(): scanner state handling lexemes starting with
 * a point.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  any of T_TR, T_ELEM_POW, T_PLUS, T_MINUS, T_ELEM_MUL, T_ELEM_DIV,
 *  T_ELEM_LDIV, T_POINT, or T_FLOAT.
 */
static ScannerToken state_point (Scanner s) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* handle operators beginning with a point. */
  switch (look) {
    case '\'': return T_TR;
    case '^':  return T_ELEM_POW;
    case '+':  return T_PLUS;
    case '-':  return T_MINUS;
    case '*':  return T_ELEM_MUL;
    case '/':  return T_ELEM_DIV;
    case '\\': return T_ELEM_LDIV;
  }

  /* if the lookahead is a digit, move into the float state. */
  if (char_is_digit(look))
    return state_float(s);

  /* if nothing else, push back a character and return T_POINT. */
  pushback(s, 1);
  return T_POINT;
}

/* state_plus(): scanner state handling lexemes starting with a plus sign.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  any of T_DEC, T_EQ_MINUS.
 */
static ScannerToken state_plus (Scanner s) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* check for the increment and assignment-addition operators. */
  switch (look) {
    case '+': return T_INC;
    case '=': return T_EQ_PLUS;
  }

  /* if nothing else, push back a character and return T_PLUS. */
  pushback(s, 1);
  return T_PLUS;
}

/* state_minus(): scanner state handling lexemes starting with
 * a minus sign.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  any of T_DEC, T_EQ_MINUS.
 */
static ScannerToken state_minus (Scanner s) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* check for the decrement and assignment-subtraction operators. */
  switch (look) {
    case '-': return T_DEC;
    case '=': return T_EQ_MINUS;
  }

  /* if nothing else, push back a character and return T_MINUS. */
  pushback(s, 1);
  return T_MINUS;
}

/* state_op(): scanner state handling operators that can be combined
 * with an assignment. also handles T_NOT and T_NE.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *  @without: token to return for non-assignment.
 *  @with: token to return for combined assignment.
 *
 * returns:
 *  if lookahead contains an equals sign, return @with.
 *  otherwise, return @without.
 */
static ScannerToken state_op (Scanner s,
                              ScannerToken without,
                              ScannerToken with) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* if the lookahead is an equals sign, return the long token. */
  if (look == '=')
    return with;

  /* if no equals sign, push back a character and return the short token. */
  pushback(s, 1);
  return without;
}

/* state_doub(): scanner state handling 'single-tap' and 'double-tap'
 * operators not already handled by their own states.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *  @prev: character to check for double-taps.
 *  @one: token to return for single-taps.
 *  @two: token to return for double-taps.
 *
 * returns:
 *  if lookahead contains another @prev, return @two. otherwise, return @one.
 */
static ScannerToken state_doub (Scanner s, char prev,
                                ScannerToken one,
                                ScannerToken two) {
  /* read a single character of lookahead. */
  char look = lookahead(s, 1);

  /* if a double-tap is found, return the long token. */
  if (look == prev)
    return two;

  /* if no double-tap, push back a character and return the short token. */
  pushback(s, 1);
  return one;
}

/* state_id(): scanner state handling identifiers and keywords.
 *
 * arguments:
 *  @s: matte scanner to utilize.
 *
 * returns:
 *  either T_IDENT or a token corresponding to any of the
 *  reserved keywords present in reserved[], defined above.
 */
static ScannerToken state_id (Scanner s) {
  /* declare required variables:
   *  @look: lookahead character.
   *  @i: reserved keyword array index.
   */
  char look;
  int i;

  /* read a single character of lookahead. */
  look = lookahead(s, 1);

  /* loop until no more valid identifier characters remain. */
  while (char_is_ident(look))
    look = lookahead(s, 1);

  /* push back one character of lookahead (from the while loop above)
   * and compute the length of the identifier's lexeme.
   */
  pushback(s, 1);
  term(s);

  /* search the reserved words array for the current lexeme. */
  for (i = 0; reserved[i].lex; i++) {
    /* check if the current reserved lexeme is a match. */
    if (strcmp(s->tok, reserved[i].lex) == 0) {
      unterm(s);
      return reserved[i].tok;
    }
  }

  /* not reserved. return an identifier. */
  unterm(s);
  return T_IDENT;
}

/* consume_whitespace(): move past any whitespace that is present
 * at the current token end.
 *
 * arguments:
 *  @s: matte scanner to access.
 */
static inline void consume_whitespace (Scanner s) {
  /* treat spaces and tabs as whitespace. */
  while (char_is_whitespace(*s->tok_end)) {
    lookahead(s, 1);
    s->tok++;
  }
}

/* consume_nonprinting(): move past any nonprinting inputs
 * (e.g., comments, whitespace, and line continuations)
 * that are present at the current token end.
 *
 * arguments:
 *  @s: matte scanner to access.
 */
static inline void consume_nonprinting (Scanner s) {
  /* consume any whitespace. */
  consume_whitespace(s);

  /* consume line-continuation tokens. this will not remove
   * usages of "..." as an inline comment.
   */
  if (s->buf_end - s->tok_end > 3 &&
      *s->tok_end == '.' &&
      *(s->tok_end + 1) == '.' &&
      *(s->tok_end + 2) == '.' &&
      *(s->tok_end + 3) == '\n') {
    /* skip past the continuation and increment the line number. */
    lookahead(s, 4);
    s->lineno++;
  }

  /* consume any whitespace. */
  consume_whitespace(s);

  /* check for comments. */
  if (s->buf_end - s->tok_end > 1 &&
      *s->tok_end == '%' &&
      *(s->tok_end + 1) == '{') {
    /* consume multi-line comments. */
    while (lookahead(s, 1) &&
           *s->tok_end != '%' &&
            *(s->tok_end + 1) != '}') {
      /* also move the token start pointer to allow buffer flushes. */
      s->tok++;

      /* increment the line number when necessary. */
      if (*s->tok_end == '\n')
        s->lineno++;
    }

    /* skip past the comment end token. */
    lookahead(s, 2);
  }
  else if (*s->tok_end == '%') {
    /* consume single-line comments. */
    while (*s->tok_end != '\n' && lookahead(s, 1));
  }

  /* consume any whitespace. */
  consume_whitespace(s);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* scanner_type(): return a pointer to the scanner object type.
 */
ObjectType scanner_type (void) {
  /* return the struct pointer. */
  return &Scanner_type;
}

/* scanner_new(): allocate a new matte scanner.
 *
 * returns:
 *  newly allocated and initialized matte scanner.
 */
Scanner scanner_new (Object args) {
  /* allocate a new scanner. */
  Scanner s = (Scanner) Scanner_type.fn_alloc(&Scanner_type);
  if (!s)
    return NULL;

  /* set the filename and file descriptor. */
  s->fname = NULL;
  s->fd = -1;

  /* initialize the buffer string. */
  s->buf = s->buf_end = NULL;
  s->buf_size = 0L;
  s->buf_pos = 0L;

  /* initialize the current token. */
  s->tok = s->tok_end = NULL;

  /* initialize the line number and error count. */
  s->lineno = 1L;
  s->err = 0L;

  /* return the new scanner. */
  return s;
}

/* scanner_new_with_file(): allocate a new matte scanner associated with
 * a filename string.
 *
 * arguments:
 *  @fname: input filename string.
 *
 * returns:
 *  newly allocated and initialized scanner.
 */
Scanner scanner_new_with_file (const char *fname) {
  /* declare required variables:
   *  @s: new scanner to allocate.
   */
  Scanner s;

  /* allocate a new scanner and set the filename string. */
  s = scanner_new(NULL);
  if (!s || !scanner_set_file(s, fname)) {
    object_free((Object) s);
    return NULL;
  }

  /* return the new scanner. */
  return s;
}

/* scanner_new_with_string(): allocate a new matte scanner associated with
 * a buffer string.
 *
 * arguments:
 *  @str: input buffer string.
 *
 * returns:
 *  newly allocated and initialized scanner.
 */
Scanner scanner_new_with_string (const char *str) {
  /* declare required variables:
   *  @s: new scanner to allocate.
   */
  Scanner s;

  /* allocate a new scanner and set the string buffer. */
  s = scanner_new(NULL);
  if (!s || !scanner_set_string(s, str)) {
    object_free((Object) s);
    return NULL;
  }

  /* return the new scanner. */
  return s;
}

/* scanner_close(): close the input stream of a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to close.
 */
static void scanner_close (Scanner s) {
  /* check if the filename string is allocated. */
  if (s->fname)
    free(s->fname);

  /* check if the scanner contains an open file. */
  if (s->fd >= 0)
    close(s->fd);

  /* reset the filename and file descriptor. */
  s->fname = NULL;
  s->fd = -1;

  /* free the scanner buffer. */
  if (s->buf)
    free(s->buf);

  /* reset the buffer string. */
  s->buf = s->buf_end = NULL;
  s->buf_size = 0L;
  s->buf_pos = 0L;

  /* reset the current token. */
  s->tok = s->tok_end = NULL;

  /* reset the line number and error count. */
  s->lineno = 1L;
  s->err = 0L;
}

/* scanner_free(): free all memory associated with a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to free.
 */
void scanner_free (Scanner s) {
  /* return if the scanner is null. */
  if (!s)
    return;

  /* close the scanner input stream. */
  scanner_close(s);
}

/* scanner_set_file(): set the filename string of a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to modify.
 *  @fname: new filename string to set.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int scanner_set_file (Scanner s, const char *fname) {
  /* declare required variables:
   *  @n: number of bytes read from the input file.
   */
  long n;

  /* validate the input arguments. */
  if (!s || !fname)
    fail("invalid input arguments");

  /* close any open input files. */
  scanner_close(s);

  /* attempt to open the file. */
  s->fd = open(fname, O_RDONLY);
  if (s->fd < 0) {
    scanner_close(s);
    fail("unable to open '%s' for reading", fname);
  }

  /* store the filename string. */
  s->fname = strdup(fname);
  if (!s->fname) {
    scanner_close(s);
    fail("unable to set filename");
  }

  /* initialize the buffer size. */
  s->buf_size = SCANNER_BUF_SIZE;

  /* allocate the initial buffer. */
  s->buf = (char*) malloc(s->buf_size * sizeof(char));
  if (!s->buf) {
    scanner_close(s);
    fail("unable to allocate buffer");
  }

  /* initialize the buffer contents. */
  memset(s->buf, 0, s->buf_size * sizeof(char));

  /* set the buffer end pointer. */
  s->buf_end = s->buf + s->buf_size;

  /* initialize the token. */
  s->tok = s->buf;
  s->tok_end = s->buf;

  /* perform an initial read of the input file. */
  n = read(s->fd, s->buf, s->buf_size);

  /* check if the file contents were exhausted. */
  if (n < s->buf_size) {
    /* update the buffer size. */
    s->buf_end = s->buf + n;
    s->buf_size = n;

    /* close the input file. */
    close(s->fd);
    s->fd = -1;
  }

  /* return success. */
  return 1;
}

/* scanner_set_string(): set the buffer string of a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to modify.
 *  @str: new buffer string to set.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int scanner_set_string (Scanner s, const char *str) {
  /* validate the input arguments. */
  if (!s || !str)
    fail("invalid input arguments");

  /* close any open input files. */
  scanner_close(s);

  /* duplicate the input string into the buffer. */
  s->buf = strdup(str);
  if (!s->buf)
    fail("unable to copy buffer");

  /* set the buffer length and end pointer. */
  s->buf_size = strlen(s->buf);
  s->buf_end = s->buf + s->buf_size;

  /* initialize the token. */
  s->tok = s->buf;
  s->tok_end = s->buf;

  /* return success. */
  return 1;
}

/* scanner_get_filename(): get a constant string representation of the
 * input character source being read by a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  if the scanner is reading a file, the filename string is returned.
 *  if a string is being scanned, then "(string)" is returned.
 */
const char *scanner_get_filename (Scanner s) {
  /* return a string indicating the input source of the scanner. */
  return (s ? (s->fname ? s->fname : "(string)") : "(unknown)");
}

/* scanner_get_lineno(): get the current line number of a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  current line number where the scanner is reading characters.
 */
long scanner_get_lineno (Scanner s) {
  /* return the line number of the scanner, if possible. */
  return (s ? (s->tok && *s->tok == '\n' ? s->lineno - 1L : s->lineno) : 1L);
}

/* scanner_get_pos(): get the current cursor position of a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  current byte number where the scanner cursor is located.
 */
long scanner_get_pos (Scanner s) {
  /* return the position of the scanner, if possible. */
  return (s && s->buf && s->tok && s->tok < s->buf_end ?
          s->buf_pos + (s->tok - s->buf) : 0L);
}

/* scanner_get_linestr(): get a string indicating the current position
 * of a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  current line string. must be deallocated by the caller.
 */
char *scanner_get_linestr (Scanner s) {
  /* declare required variables:
   *  @pa, @pb: string pointers for the start and end of the line.
   *  @str: output line string.
   *  @i: general-purpose loop counter.
   *  @n: number of characters in the current line.
   */
  char *pa, *pb, *str;
  long i, n;

  /* return if the scanner is null. */
  if (!s)
    return NULL;

  /* initialize the start and end pointers. */
  pa = pb = s->tok;
  if (*s->tok == '\n') {
    pa--;
    pb--;
  }

  /* locate the start of the line. */
  while (pa >= s->buf && *pa != '\n')
    pa--;

  /* locate the end of the line. */
  while (pb < s->buf_end && *pb != '\n')
    pb++;

  /* adjust the start and end pointers. */
  pa++;
  pb--;

  /* compute the string length. */
  n = pb - pa + 1L;

  /* allocate the line string. */
  str = (char*) malloc((2L * n + 4L) * sizeof(char));
  if (!str)
    return NULL;

  /* copy in the string. */
  strcpy(str, " ");
  strncat(str, pa, n);
  strcat(str, "\n");

  /* place a cursor at the current scanner position. */
  n = s->tok - pa;
  for (i = 0; i <= n; i++)
    strcat(str, " ");

  /* place the cursor and add a newline. */
  strcat(str, "^\n");

  /* return the constructed string. */
  return str;
}

/* scanner_get_errors(): get the number of errors reported by a matte scanner.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  number of errors in the scanner, if any.
 */
long scanner_get_errors (Scanner s) {
  /* return the number of errors reported by the scanner.
   * if the scanner pointer is invalid, return an error.
   */
  return (s ? s->err : 1L);
}

/* scanner_get_string(): get a copy of the current token's lexeme. the
 * user is responsible for freeing the string after use.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  duplicate string holding the current token's lexeme. must be
 *  free'd after use.
 */
char *scanner_get_string (Scanner s) {
  /* declare required variables:
   *  @stok: duplicated token string.
   */
  char *stok;

  /* temporarily null-terminate the token, duplicate the string,
   * and then un-terminate the token.
   */
  term(s);
  stok = strdup(s->tok);
  unterm(s);

  /* return the duplicated string. */
  return stok;
}

/* scanner_get_int(): get an integer representation of the current token.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  long integer translated from the current token's lexeme.
 */
long scanner_get_int (Scanner s) {
  /* declare required variables:
   *  @val: output value.
   */
  long val;

  /* translate the current token. */
  term(s);
  val = strtol(s->tok, (char**) NULL, 10);
  unterm(s);

  /* return the translated token value. */
  return val;
}

/* scanner_get_float(): get a float representation of the current token.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  double-precision float translated from the current token's lexeme.
 */
double scanner_get_float (Scanner s) {
  /* declare required variables:
   *  @val: output value.
   */
  double val;

  /* translate the current token. */
  term(s);
  val = strtod(s->tok, (char**) NULL);
  unterm(s);

  /* return the translated token value. */
  return val;
}

/* scanner_get_complex(): get a complex float representation of the current
 * token.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  double-precision complex float translated from the current token's
 *  lexeme.
 */
complex double scanner_get_complex (Scanner s) {
  /* return the translated float token value as an imaginary. */
  return scanner_get_float(s) * I;
}

/* scanner_next(): determine the next token present in a scanned input
 * stream. this is equivalent to the deterministic finite automaton
 * start state.
 *
 * arguments:
 *  @s: matte scanner to access.
 *
 * returns:
 *  next token from a scanned input stream. unknown input will be returned
 *  as the default token, T_ERR.
 */
ScannerToken scanner_next (Scanner s) {
  /* validate the input arguments. */
  if (!s)
    return T_ERR;

  /* consume whitespace, line continuations and comments. */
  consume_nonprinting(s);

  /* initialize the token start pointer. */
  if (s->tok < s->tok_end)
    s->tok = s->tok_end;

  /* initialize the token end pointer. */
  s->tok_end++;

  /* check if we've exceeded the buffer capacity. */
  if (s->tok >= s->buf_end)
    return T_EOF;

  /* get the token start character, as well as the previous character
   * in the stream, if such a character exists.
   */
  char prev = (s->tok > s->buf ? *(s->tok - 1L) : T_ERR);
  char look = *s->tok;

  /* determine the state to move into. */
  switch (look) {
    /* newline. */
    case '\n':
      s->lineno++;
      return T_EOL;

    /* unambiguous single-character tokens. */
    case '[': return T_BRACK_OPEN;
    case '{': return T_BRACE_OPEN;
    case '(': return T_PAREN_OPEN;
    case ']': return T_BRACK_CLOSE;
    case '}': return T_BRACE_CLOSE;
    case ')': return T_PAREN_CLOSE;
    case '@': return T_AS;
    case ',': return T_COMMA;
    case ':': return T_COLON;
    case ';': return T_SEMI;

    /* multiple-character tokens: string literals. */
    case '"': return state_str(s, look);

    /* multiple-character tokens: transpose and string literals. */
    case '\'':
      /* only allow single-quote strings to begin after certain characters.
       */
      if (char_is_ident(prev) || prev == ']' || prev == '}' || prev == ')')
        return T_HTR;
      else
        return state_str(s, look);

    /* multiple-character tokens: operators. */
    case '.':  return state_point(s);
    case '+':  return state_plus(s);
    case '-':  return state_minus(s);
    case '^':  return state_op(s, T_POW, T_EQ_POW);
    case '!':
    case '~':  return state_op(s, T_NOT,    T_NE);
    case '*':  return state_op(s, T_MUL,    T_EQ_MUL);
    case '/':  return state_op(s, T_DIV,    T_EQ_DIV);
    case '\\': return state_op(s, T_LDIV,   T_EQ_LDIV);
    case '<':  return state_op(s, T_LT,     T_LE);
    case '>':  return state_op(s, T_GT,     T_GE);
    case '=':  return state_op(s, T_ASSIGN, T_EQ);
    case '&':  return state_doub(s, look, T_ELEM_AND, T_AND);
    case '|':  return state_doub(s, look, T_ELEM_OR,  T_OR);
  }

  /* multiple-character tokens: keywords and identifiers. */
  if ((look >= 'a' && look <= 'z') ||
      (look >= 'A' && look <= 'Z'))
    return state_id(s);

  /* multiple-character tokens: integers and floats. */
  if (char_is_digit(look))
    return state_int(s);

  /* return an unknown token. */
  pushback(s, 1);
  return T_ERR;
}

/* Scanner_type: object type structure for matte scanners.
 */
struct _ObjectType Scanner_type = {
  "Scanner",                                     /* name       */
  sizeof(struct _Scanner),                       /* size       */
  0,                                             /* precedence */

  (obj_constructor) scanner_new,                 /* fn_new     */
  NULL,                                          /* fn_copy    */
  (obj_allocator)   object_alloc,                /* fn_alloc   */
  (obj_destructor)  scanner_free,                /* fn_dealloc */
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

