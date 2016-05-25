
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_SCANNER_H__
#define __MATTE_SCANNER_H__

/* include the object header. */
#include <matte/object.h>

/* include the variable-length argument header. */
#include <stdarg.h>

/* include the low-level i/o headers. */
#include <unistd.h>
#include <fcntl.h>

/* include the matte scanner token header. */
#include <matte/scanner-token.h>

/* IS_SCANNER: macro to check that an object is a matte scanner.
 */
#define IS_SCANNER(obj) \
  MATTE_TYPE_CHECK(obj, scanner_type())

/* Scanner: pointer to a struct _Scanner. */
typedef struct _Scanner *Scanner;
struct _ObjectType Scanner_type;

/* Scanner: structure for holding the current state of a matte
 * lexical analyzer.
 *
 * the scanner takes input from two primary sources: files (accessed
 * via a file descriptor and associated with a filename string), and
 * strings (which are just treated as files that have been completely
 * read into the scan buffer.
 */
struct _Scanner {
  /* base object. */
  OBJECT_BASE;

  /* @fname: filename string of the input file, if any.
   * @fd: file descriptor of the input file, if any.
   */
  char *fname;
  int fd;

  /* @buf: buffer string to be scanned.
   * @buf_end: end of the buffer string.
   * @buf_size: number of valid characters in the buffer.
   * @buf_pos: offset position of the buffer in the file, if any.
   */
  char *buf, *buf_end;
  long buf_size, buf_pos;

  /* @tok: start of the current token/lexeme.
   * @tok_end: end of the current token/lexeme.
   * @term: temporary storage while null-terminating tokens.
   */
  char *tok, *tok_end;
  char term;

  /* @lineno: current line number in the input stream.
   * @err: number of errors encountered by the scanner.
   */
  long lineno, err;
};

/* function declarations (scanner.c): */

ObjectType scanner_type (void);

Scanner scanner_new (Object args);

Scanner scanner_new_with_file (const char *fname);

Scanner scanner_new_with_string (const char *str);

void scanner_free (Scanner s);

int scanner_set_file (Scanner s, const char *fname);

int scanner_set_string (Scanner s, const char *str);

const char *scanner_get_filename (Scanner s);

long scanner_get_lineno (Scanner s);

long scanner_get_pos (Scanner s);

char *scanner_get_linestr (Scanner s);

long scanner_get_errors (Scanner s);

char *scanner_get_string (Scanner s);

long scanner_get_int (Scanner s);

double scanner_get_float (Scanner s);

complex double scanner_get_complex (Scanner s);

ScannerToken scanner_next (Scanner s);

#endif /* !__MATTE_SCANNER_H__ */

