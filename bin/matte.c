
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the matte library headers. */
#include <matte/matte.h>
#include <matte/compiler.h>

/* main(): compiler entry point.
 *
 * arguments:
 *  @argc: command line argument count.
 *  @argv: command line argument array.
 *
 * returns:
 *  integer indicating compilation success (0) or failure (!0).
 */
int main (int argc, char **argv) {
  /* declare option parsing arguments:
   *  @argi: argument array index.
   *  @arg: current argument string.
   *  @arglen: length of the current argument string.
   */
  int argi, arglen;
  char *arg;

  /* allocate a new compiler object. */
  Compiler compiler = compiler_new(NULL);

  /* loop over the arguments. */
  argi = 1;
  while (argi < argc) {
    arg = argv[argi];
    arglen = strlen(arg);

    if (arglen >= 2 && arg[0] == '-') {
      if (arg[1] == 'g' || arg[1] == 'f' ||
          arg[1] == 'm' || arg[1] == 'O' ||
          arg[1] == 'W' || arg[1] == 'L') {
        if (!compiler_add_cflag(compiler, arg))
          die("unable to add compiler flag");
      }
      else if (arg[1] == 'I') {
        if (arglen >= 3 && !compiler_add_path(compiler, arg + 2))
          die("unable to add pathname string");
      }
      else if (arg[1] == 'c') {
        if (!compiler_set_mode(compiler, COMPILE_TO_C))
          die("unable to set compiler mode");
      }
      else if (arg[1] == 'o') {
        if (argi == argc - 1)
          die("expected output filename argument");

        if (compiler->mode == COMPILE_TO_MEM &&
            !compiler_set_mode(compiler, COMPILE_TO_EXE))
          die("unable to set compiler mode");

        argi++;
        arg = argv[argi];
        if (!compiler_set_outfile(compiler, arg))
          die("unable to set output filename");
      }
      else die("unsupported argument %s", arg);
    }
    else {
      if (!compiler_add_file(compiler, arg))
        die("failed to compile '%s'", arg);
    }

    argi++;
  }

  /* execute the compilation as configured. */
  if (!compiler_execute(compiler))
    die("failed to perform compilation");

  /* free the compiler object. */
  object_free((Object) compiler);

  /* return success. */
  return 0;
}

