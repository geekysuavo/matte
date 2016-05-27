
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* ensure once-only inclusion. */
#ifndef __MATTE_STRING_H__
#define __MATTE_STRING_H__

/* include the object headers. */
#include <matte/object.h>
#include <matte/object-list.h>

/* include required standard c library headers. */
#include <ctype.h>

/* IS_STRING: macro to check that an object is a matte string.
 */
#define IS_STRING(obj) \
  MATTE_TYPE_CHECK(obj, string_type())

/* String: pointer to a struct _String. */
typedef struct _String *String;
struct _ObjectType String_type;

/* _String: structure for holding a vector of characters.
 */
struct _String {
  /* base object. */
  OBJECT_BASE;

  /* @data: array of string characters.
   * @n: number of string characters.
   */
  char *data;
  int n;
};

/* function declarations (string.c): */

ObjectType string_type (void);

String string_new (Object args);

String string_new_with_length (int n);

String string_new_with_value (const char *str);

String string_copy (String s);

void string_free (String s);

int string_get_length (String s);

const char *string_get_value (String s);

int string_set_length (String s, int n);

int string_set_value (String s, const char *str);

int string_append_value (String s, const char *suf);

int string_appendf (String s, const char *format, ...);

int string_append (String s, String suffix);

int string_trim (String s);

int string_trim_left (String s);

int string_trim_right (String s);

int string_tolower (String s);

int string_toupper (String s);

int string_cmp (String s1, String s2);

ObjectList string_split (String s, String pat);

/* function declarations (builtins.c): */

int string_append_objs (String s, char *format, int begin, ObjectList lst);

#endif /* !__MATTE_STRING_H__ */

