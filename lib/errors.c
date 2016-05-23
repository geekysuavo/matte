
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the errors header. */
#include <matte/errors.h>

/* matte_error: structure for holding error message information.
 */
struct matte_error {
  /* @line: line number where the error was emitted.
   * @fname: source filename that emitted the error.
   * @num: emitted error code, or 0 if none.
   */
  unsigned int line;
  char *fname;
  int num;

  /* @msg: custom error message string, or NULL if none.
   */
  char *msg;
};

/* @errv: global (yet private) array of error traceback information.
 * @errn: number of errors in the global @errv array.
 */
struct matte_error *errv;
int errn = -1;

/* matte_errors_init(): initialize the traceback array for use.
 */
void matte_errors_init (void) {
  /* return if the array is already initialized. */
  if (errn >= 0)
    return;

  /* initialize the traceback array. */
  errv = NULL;
  errn = 0;
}

/* matte_errors_print(): print the contents of the traceback array to
 * standard error.
 */
void matte_errors_print (void) {
  /* declare required variables:
   *  @i: error array index.
   */
  int i;

  /* initialize the traceback array. */
  matte_errors_init();

  /* flush standard output. */
  fflush(stdout);

  /* loop over the traceback array. */
  for (i = 0; i < errn; i++) {
    /* print the initial line of information. */
    fprintf(stderr, "[%d] %s:%u:\n", i, errv[i].fname, errv[i].line);

    /* print the custom message string, if available. */
    if (errv[i].msg)
      fprintf(stderr, "  %s\n", errv[i].msg);

    /* print the error code interpretation, if available. */
    if (errv[i].num)
      fprintf(stderr, "  (%s)\n", strerror(errv[i].num));
  }

  /* flush standard error. */
  fflush(stderr);
}

/* matte_errors_reset(): empty the traceback array.
 */
void matte_errors_reset (void) {
  /* declare required variables:
   *  @i: error array index.
   */
  int i;

  /* initialize the traceback array. */
  matte_errors_init();

  /* loop over the traceback array. */
  for (i = 0; i < errn; i++) {
    /* free the filename string, if allocated. */
    if (errv[i].fname)
      free(errv[i].fname);

    /* free the custom message string, if allocated. */
    if (errv[i].msg)
      free(errv[i].msg);
  }

  /* free the traceback array. */
  free(errv);
  errv = NULL;

  /* reset the error code and traceback array size. */
  errno = 0;
  errn = 0;
}

/* matte_errors_add(): append another error information frame to the
 * traceback array.
 *
 * arguments:
 *  @fname: the source file of error emission.
 *  @line: the line number of error emission.
 *  @format: printf-style format string for custom error messages.
 *  @...: arguments that accompany the format string.
 */
void matte_errors_add (const char *fname, unsigned int line,
                       const char *format, ...) {
  /* declare required variables:
   *  @i: traceback array index of the new error frame.
   *  @nmsg: buffer size of the custom message string.
   *  @nprint: number of characters in the custom message string.
   *  @vl: variable-length arguments list for custom message strings.
   */
  int i, nmsg, nprint;
  va_list vl;

  /* initialize the traceback array. */
  matte_errors_init();

  /* increase the size of the traceback array. */
  i = errn;
  errn++;

  /* reallocate the traceback array. */
  errv = (struct matte_error*)
    realloc(errv, errn * sizeof(struct matte_error));

  /* check if reallocation failed. */
  if (!errv) {
    /* flush standard output. */
    fflush(stdout);

    /* print an immediate error message. */
    fprintf(stderr, "[x] %s:%u:\n", __FILE__, __LINE__);
    fprintf(stderr, "  unable to reallocate traceback array\n");

    /* if available, print an immediate error code translation. */
    if (errno)
      fprintf(stderr, "  (%s)\n", strerror(errno));

    /* flush standard error and return. */
    fflush(stderr);
    return;
  }

  /* store the error code. */
  errv[i].num = errno;
  errno = 0;

  /* store the error line number and initialize the message string. */
  errv[i].line = line;
  errv[i].msg = NULL;

  /* allocate memory for the filename string. */
  nmsg = strlen(fname) + 1;
  errv[i].fname = (char*) malloc(nmsg * sizeof(char));

  /* store the filename string. */
  if (errv[i].fname)
    strcpy(errv[i].fname, fname);

  /* check if a format string was provided. */
  if (format) {
    /* initialize the message string buffer size. */
    nmsg = strlen(format);

    /* loop until the entire message string was printed. */
    do {
      /* double the size of the message string buffer. */
      nmsg *= 2;
      errv[i].msg = (char*) realloc(errv[i].msg, nmsg * sizeof(char));

      /* check if reallocation failed. */
      if (!errv[i].msg) {
        /* flush standard output. */
        fflush(stdout);

        /* print an immediate error message. */
        fprintf(stderr, "[x] %s:%u:\n", __FILE__, __LINE__);
        fprintf(stderr, "  unable to allocate traceback message string\n");

        /* print an immediate error code translation, if available. */
        if (errno)
          fprintf(stderr, "  (%s)\n", strerror(errno));

        /* flush standard error and return. */
        fflush(stderr);
        return;
      }

      /* print the message string into the buffer. */
      va_start(vl, format);
      nprint = vsnprintf(errv[i].msg, nmsg, format, vl);
      va_end(vl);
    }
    while (nprint >= nmsg);
  }
}

