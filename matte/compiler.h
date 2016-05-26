
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_COMPILER_H__
#define __MATTE_COMPILER_H__

/* include the object header. */
#include <matte/object.h>

/* include the matte parser and string headers. */
#include <matte/parser.h>
#include <matte/string.h>

/* include the unistd and dlfcn headers. */
#include <unistd.h>
#include <dlfcn.h>

/* MATTEPATH_ENV_STRING: string accessed for reading path information
 * from the environment during compilation.
 */
#define MATTEPATH_ENV_STRING "MATTEPATH"

/* IS_COMPILER: macro to check that an object is a matte compiler.
 */
#define IS_COMPILER(obj) \
  MATTE_TYPE_CHECK(obj, compiler_type())

/* Compiler: pointer to a struct _Compiler. */
typedef struct _Compiler *Compiler;
struct _ObjectType Compiler_type;

/* CompilerMode: an enum _CompilerMode. */
typedef enum _CompilerMode CompilerMode;

/* _CompilerMode: enumeration that holds all basic operation modes
 * supported by the matte compiler object.
 */
enum _CompilerMode {
  COMPILE_TO_C = 0,    /* write c source code to a file. */
  COMPILE_TO_EXE,      /* build an executable binary file. */
  COMPILE_TO_MEM       /* build and dlopen() the result. */
};

/* Compiler: structure for holding the current state of a matte compiler.
 */
struct _Compiler {
  /* base object. */
  OBJECT_BASE;

  /* @par: matte parser used for reading files into a syntax tree.
   * @tree: abstract syntax tree used to store parsed files and strings.
   */
  Parser par;
  AST tree;

  /* @path: list of strings for each directory in the matte path.
   * @src: list of source files parsed by the compiler.
   */
  ObjectList path;
  ObjectList src;

  /* @mode: compilation mode enumeration value.
   * @fname: output filename, if requested.
   * @cflags: c compiler flag string.
   * @ccode: output c source code.
   */
  CompilerMode mode;
  String fout, cflags, ccode;

  /* @err: number of errors encountered by the compiler.
   */
  long err;
};

/* function declarations (compiler.c): */

ObjectType compiler_type (void);

Compiler compiler_new (Object args);

void compiler_free (Compiler c);

int compiler_set_mode (Compiler c, CompilerMode mode);

int compiler_set_outfile (Compiler c, const char *fname);

int compiler_add_cflag (Compiler c, const char *str);

int compiler_add_path (Compiler c, const char *fname);

int compiler_add_file (Compiler c, const char *fname);

int compiler_add_string (Compiler c, const char *str);

int compiler_execute (Compiler c);

#endif /* !__MATTE_COMPILER_H__ */

