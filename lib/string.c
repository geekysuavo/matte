
/* Copyright (c) 2016 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License
 */

/* include the string header. */
#include <matte/string.h>

/* string_type(): return a pointer to the string object type.
 */
ObjectType string_type (void) {
  /* return the struct address. */
  return &String_type;
}

/* string_new(): allocate a new empty matte string.
 *
 * returns:
 *  newly allocated empty string.
 */
String string_new (Object args) {
  /* allocate a new string. */
  String s = (String) String_type.fn_alloc(&String_type);
  if (!s)
    return NULL;

  /* initialize the string length. */
  s->n = 0;

  /* allocate the string data. */
  s->data = (char*) malloc(sizeof(char));

  /* check if allocation failed. */
  if (!s->data) {
    /* return failure. */
    object_free((Object) s);
    throw("unable to allocate array");
  }

  /* null-terminate the string data. */
  s->data[0] = '\0';

  /* return the new string. */
  return s;
}

/* string_new_with_length(): allocate a new matte string with a set
 * number of blank characters.
 *
 * arguments:
 *  @n: number of characters in the new string.
 *
 * returns:
 *  newly allocated blank string.
 */
String string_new_with_length (int n) {
  /* declare required variables:
   *  @s: new string to allocate.
   */
  String s;

  /* allocate a new empty string. */
  s = string_new(NULL);
  if (!s)
    throw("unable to allocate string");

  /* set the string length. */
  if (!string_set_length(s, n)) {
    object_free((Object) s);
    throw("unable to set string length");
  }

  /* fill the string with blanks. */
  memset(s->data, ' ', s->n);

  /* return the new string. */
  return s;
}

/* string_new_with_value(): allocate a new matte string with a set value.
 *
 * arguments:
 *  @str: initial value of the new string.
 *
 * returns:
 *  newly allocated string.
 */
String string_new_with_value (const char *str) {
  /* declare required variables:
   *  @s: new string to allocate.
   */
  String s;

  /* allocate a new empty string. */
  s = string_new(NULL);
  if (!s)
    throw("unable to allocate string");

  /* set the string value. */
  if (!string_set_value(s, str)) {
    object_free((Object) s);
    throw("unable to set string value");
  }

  /* return the new string. */
  return s;
}

/* string_copy(): allocate a new matte string from another matte string.
 *
 * arguments:
 *  @s: matte string to duplicate.
 *
 * returns:
 *  newly allocated duplicate string.
 */
String string_copy (String s) {
  /* declare required variables:
   *  @s: new string to allocate.
   */
  String snew;

  /* validate the input arguments. */
  if (!s)
    throw("invalid input arguments");

  /* allocate a new valued string. */
  snew = string_new_with_value(s->data);

  /* return the new string. */
  return snew;
}

/* string_free(): free all memory associated with a matte string.
 *
 * arguments:
 *  @s: matte string to free.
 */
void string_free (String s) {
  /* return if the string is null. */
  if (!s)
    return;

  /* free the string data. */
  if (s->data)
    free(s->data);
}

/* string_get_length(): get the length of a matte string.
 *
 * arguments:
 *  @s: matte string to access.
 *
 * returns:
 *  number of characters in the string.
 */
int string_get_length (String s) {
  /* return the string length. */
  return (s ? s->n : 0);
}

/* string_get_value(): get the raw contents of a matte string.
 *
 * arguments:
 *  @s: matte string to access.
 *
 * returns:
 *  pointer to the internal character array of the string.
 */
const char *string_get_value (String s) {
  /* return the raw data of the string. */
  return (s ? (const char*) s->data : NULL);
}

/* string_set_length(): set the length of a matte string.
 *
 * arguments:
 *  @s: matte string to modify.
 *  @n: new length of the string.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_set_length (String s, int n) {
  /* validate the input arguments. */
  if (!s || n < 0)
    fail("invalid input arguments");

  /* return if the string already has the desired length. */
  if (n == s->n)
    return 1;

  /* reallocate the string data. */
  s->data = (char*) realloc(s->data, (n + 1) * sizeof(char));
  if (!s->data)
    fail("unable to reallocate array");

  /* fill new trailing string data with blanks. */
  if (n > s->n)
    memset(s->data + s->n, ' ', n - s->n);

  /* null-terminate the string data and set the length. */
  s->data[n] = '\0';
  s->n = n;

  /* return success. */
  return 1;
}

/* string_set_value(): set the value of a matte string.
 *
 * arguments:
 *  @s: matte string to modify.
 *  @str: new value of the string.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_set_value (String s, const char *str) {
  /* validate the input arguments. */
  if (!s || !str)
    fail("invalid input arguments");

  /* store the new string length. */
  s->n = strlen(str);

  /* reallocate the string data. */
  s->data = (char*) realloc(s->data, (s->n + 1) * sizeof(char));
  if (!s->data)
    fail("unable to reallocate array");

  /* copy the new string value and null-terminate the string data. */
  strcpy(s->data, str);
  s->data[s->n] = '\0';

  /* return success. */
  return 1;
}

/* string_append_value(): append raw character data to the end of a string.
 *
 * arguments:
 *  @s: matte string to modify.
 *  @suf: suffix data to append to the string.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_append_value (String s, const char *suf) {
  /* declare required variables:
   *  @n: new string length.
   */
  int n;

  /* validate the input arguments. */
  if (!s || !suf)
    fail("invalid input arguments");

  /* determine the new string length. */
  n = s->n + strlen(suf);

  /* reallocate the string data. */
  s->data = (char*) realloc(s->data, (n + 1) * sizeof(char));
  if (!s->data)
    fail("unable to reallocate array");

  /* concatenate and null-terminate the string data. */
  strcat(s->data, suf);
  s->data[n] = '\0';

  /* store the new string length. */
  s->n = n;

  /* return success. */
  return 1;
}

/* string_appendf(): append data to the end of a string in printf-style.
 *
 * arguments:
 *  @s: matte string to modify.
 *  @format: printf-style format string.
 *  @...: arguments for the format string.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_appendf (String s, const char *format, ...) {
  /* declare required variables:
   *  @buf: buffer string for generating the appended data.
   *  @vl: variable-length arguments list for snprintf.
   *  @nbuf: number of chars in the buffer.
   *  @nout: number of chars written.
   */
  int nbuf, nout;
  va_list vl;
  char *buf;

  /* validate the input arguments. */
  if (!s || !format)
    fail("invalid input arguments");

  /* loop until the buffer contains the complete result. */
  nbuf = strlen(format);
  buf = NULL;
  do {
    /* double the buffer size and reallocate. */
    nbuf *= 2;
    buf = (char*) realloc(buf, nbuf * sizeof(char));

    /* check if reallocation failed. */
    if (!buf)
      fail("unable to reallocate buffer");

    /* print into the buffer. */
    va_start(vl, format);
    nout = vsnprintf(buf, nbuf, format, vl);
    va_end(vl);
  }
  while (nout >= nbuf);

  /* append the data to the string. */
  if (!string_append_value(s, buf))
    fail("unable to append formatted data");

  /* free the buffer and return success. */
  free(buf);
  return 1;
}

/* string_append(): append one string to the end of another.
 *
 * arguments:
 *  @s: matte string to modify.
 *  @suffix: matte string to append onto the first.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_append (String s, String suffix) {
  /* validate the input arguments. */
  if (!s || !suffix)
    fail("invalid input arguments");

  /* use the raw data append function. */
  return string_append_value(s, suffix->data);
}

/* string_trim(): trim leading and trailing whitespace from a matte string.
 *
 * arguments:
 *  @s: matte string to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_trim (String s) {
  /* declare required variables:
   *  @i, @j: left and right trim offsets.
   *  @dtmp: temporary string data variable.
   */
  int i, j;
  char *dtmp;

  /* validate the input arguments. */
  if (!s)
    fail("invalid input arguments");

  /* while the left trim offset is left of the string end and the first
   * character is whitespace, move the left offset to the right.
   */
  for (i = 0;
       i < s->n &&
       (s->data[i] == ' ' ||
        s->data[i] == '\t' ||
        s->data[i] == '\r' ||
        s->data[i] == '\n'); i++) {}

  /* while the right trim offset is right of the string start and
   * the last character is whitespace, move the right offset to
   * the left.
   */
  for (j = s->n - 1;
       j >= 0 &&
       (s->data[j] == ' ' ||
        s->data[j] == '\t' ||
        s->data[j] == '\r' ||
        s->data[j] == '\n'); j--) {}

  /* handle the special case of all-whitespace strings. */
  if (i > j)
    return string_set_length(s, 0);

  /* reallocate a new buffer for trimmed string data. */
  dtmp = (char*) malloc((j - i + 2) * sizeof(char));
  if (!dtmp)
    fail("unable to allocate array");

  /* copy the trimmed data into the new buffer. */
  strncpy(dtmp, s->data + i, j - i + 1);
  dtmp[j - i + 1] = '\0';

  /* free the existing string data and set it to the new trimmed data. */
  free(s->data);
  s->data = dtmp;

  /* store the new string length. */
  s->n = j - i + 1;

  /* return success. */
  return 1;
}

/* string_trim_left(): trim leading whitespace from a matte string.
 *
 * arguments:
 *  @s: matte string to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_trim_left (String s) {
  /* declare required variables:
   *  @i: left trim offset.
   *  @n: new string length.
   *  @dtmp: temporary string data variable.
   */
  int i, n;
  char *dtmp;

  /* validate the input arguments. */
  if (!s)
    fail("invalid input arguments");

  /* while the left trim offset is left of the string end and the first
   * character is whitespace, move the left offset to the right.
   */
  for (i = 0;
       i < s->n &&
       (s->data[i] == ' ' ||
        s->data[i] == '\t' ||
        s->data[i] == '\r' ||
        s->data[i] == '\n'); i++) {}

  /* handle the special case of all-whitespace strings. */
  if (i >= s->n)
    return string_set_length(s, 0);

  /* determine the new string length. */
  n = s->n - i;

  /* reallocate a new buffer for trimmed string data. */
  dtmp = (char*) malloc((n + 1) * sizeof(char));
  if (!dtmp)
    fail("unable to allocate array");

  /* copy the trimmed data into the new buffer. */
  strncpy(dtmp, s->data + i, n);
  dtmp[n] = '\0';

  /* free the existing string data and set it to the new trimmed data. */
  free(s->data);
  s->data = dtmp;

  /* store the new string length. */
  s->n = n;

  /* return success. */
  return 1;
}

/* string_trim_right(): trim trailing whitespace from a matte string.
 *
 * arguments:
 *  @s: matte string to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_trim_right (String s) {
  /* declare required variables:
   *  @n: new string length.
   */
  int n;

  /* validate the input arguments. */
  if (!s)
    fail("invalid input arguments");

  /* get the current string length. */
  n = s->n;

  /* while the string length is positive and the last character is
   * whitespace, decrease the string length.
   */
  while (n && 
         (s->data[n - 1] == ' ' ||
          s->data[n - 1] == '\t' ||
          s->data[n - 1] == '\r' ||
          s->data[n - 1] == '\n')) {
    n--;
  }

  /* set the new string length. */
  if (!string_set_length(s, n))
    fail("unable to set string length");

  /* return success. */
  return 1;
}

/* string_tolower(): convert a matte string to lowercase.
 *
 * arguments:
 *  @s: matte string to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_tolower (String s) {
  /* declare required variables:
   *  @i: string data loop counter.
   */
  int i;

  /* validate the input arguments. */
  if (!s)
    fail("invalid input arguments");

  /* convert each character in the string data to uppercase. */
  for (i = 0; i < s->n; i++)
    s->data[i] = (char) tolower ((int) s->data[i]);

  /* return success. */
  return 1;
}

/* string_toupper(): convert a matte string to uppercase.
 *
 * arguments:
 *  @s: matte string to modify.
 *
 * returns:
 *  integer indicating success (1) or failure (0).
 */
int string_toupper (String s) {
  /* declare required variables:
   *  @i: string data loop counter.
   */
  int i;

  /* validate the input arguments. */
  if (!s)
    fail("invalid input arguments");

  /* convert each character in the string data to uppercase. */
  for (i = 0; i < s->n; i++)
    s->data[i] = (char) toupper ((int) s->data[i]);

  /* return success. */
  return 1;
}

/* string_cmp(): compare two matte strings by content.
 *
 * arguments:
 *  @s1: first matte string.
 *  @s2: second matte string.
 *
 * returns:
 *  integer indicating string equality (1) or inequality (0). these
 *  return values follow the established MATLAB strcmp() convention.
 */
int string_cmp (String s1, String s2) {
  /* check that both strings are valid. */
  if (!s1 || !s2)
    return 0;

  /* check for string length equality. */
  if (s1->n != s2->n)
    return 0;

  /* check for equality. */
  if (strcmp(s1->data, s2->data) == 0)
    return 1;

  /* return inequality. */
  return 0;
}

/* string_split(): split a string into multiple strings based on a
 * delimiter string.
 *
 * arguments:
 *  @s: matte string to split.
 *  @pat: matte string to use as a delimiter.
 *
 * returns:
 *  matte object list containing each individual string produced
 *  by the split operation, or NULL on failure.
 */
ObjectList string_split (String s, String pat) {
  /* declare required variables:
   *  @pa: token string start pointer.
   *  @pb: token string end pointer.
   *  @buf: temporary string buffer.
   *  @n: current token string length.
   *  @done: tokenization loop completion flag.
   */
  char *pa, *pb, *buf;
  int n, done;

  /* allocate a new object list. */
  ObjectList lst = object_list_new(NULL);
  if (!lst)
    return NULL;

  /* allocate the buffer string. */
  buf = (char*) malloc((s->n + 1) * sizeof(char));

  /* check if allocation failed. */
  if (!buf) {
    /* free the object list and return failure. */
    object_free((Object) lst);
    throw("unable to allocate array");
  }

  /* initialize the first token start pointer. */
  pa = s->data;

  /* initialize the completion flag. */
  done = 0;

  /* loop until completion. */
  do {
    /* find the next token end pointer. */
    pb = strstr(pa, pat->data);

    /* check if a new pointer was not found. */
    if (!pb) {
      /* this is the last token. */
      done = 1;
      pb = pa + strlen(pa);
    }

    /* compute the token string length. */
    n = pb - pa;

    /* copy the current token string into the buffer. */
    strncpy(buf, pa, n);
    buf[n] = '\0';

    /* append a new matte [token] string into the object list. */
    if (!object_list_append(lst, (Object) string_new_with_value(buf))) {
      /* free the object list and buffer string. */
      object_free((Object) lst);
      free(buf);

      /* return failure. */
      throw("unable to append string to object array");
    }

    /* move the token start pointer past the last delimiter. */
    pa = pb + strlen(pat->data);
  }
  while (!done);

  /* free the buffer string and return the object list. */
  free(buf);
  return lst;
}

/* string_disp(): display function for strings.
 * see String_type for more detailed information.
 */
int string_disp (String s, const char *var) {
  printf("%s = '%s'\n", var, s->data);
  return 1;
}

/* string_horzcat(): horizontal concatenation function for strings.
 * see String_type for more detailed information.
 */
String string_horzcat (int n, va_list vl) {
  /* declare required variables:
   *  @scat: output concatenation result.
   *  @si: current input string.
   *  @i: current input index.
   */
  String scat, si;
  int i;

  /* allocate an output string. */
  scat = string_new(NULL);
  if (!scat)
    return NULL;

  /* loop over the variable arguments list. */
  for (i = 0; i < n; i++) {
    si = (String) va_arg(vl, String);

    /* append the input string to the output string. */
    if (!IS_STRING(si) || !string_append(scat, si)) {
      object_free((Object) scat);
      return NULL;
    }
  }

  /* end the variable arguments list and return the result. */
  return scat;
}

/* string_vertcat(): vertical concatenation function for strings.
 * see String_type for more detailed information.
 */
String string_vertcat (int n, va_list vl) {
  /* declare required variables:
   *  @scat: output concatenation result.
   *  @si: current input string.
   *  @i: current input index.
   */
  String scat, si;
  int i;

  /* allocate an output string. */
  scat = string_new(NULL);
  if (!scat)
    return NULL;

  /* loop over the variable arguments list. */
  for (i = 0; i < n; i++) {
    /* extract the current argument. */
    si = (String) va_arg(vl, String);

    /* append the input string to the output string. */
    if (!IS_STRING(si) || !string_append(scat, si)) {
      object_free((Object) scat);
      return NULL;
    }

    /* insert a newline between input strings. */
    if (i < n - 1) {
      si = string_new_with_value("\n");
      string_append(scat, si);
      object_free((Object) si);
    }
  }

  /* end the variable arguments list and return the result. */
  return scat;
}

/* String_type: object type structure for matte strings.
 */
struct _ObjectType String_type = {
  "String",                            /* name       */
  sizeof(struct _String),              /* size       */
  0,                                   /* precedence */

  (obj_constructor) string_new,        /* fn_new     */
  (obj_allocator)   object_alloc,      /* fn_alloc   */
  (obj_destructor)  string_free,       /* fn_dealloc */
  (obj_display)     string_disp,       /* fn_disp    */

  NULL,                                /* fn_plus       */
  NULL,                                /* fn_minus      */
  NULL,                                /* fn_uminus     */
  NULL,                                /* fn_times      */
  NULL,                                /* fn_mtimes     */
  NULL,                                /* fn_rdivide    */
  NULL,                                /* fn_ldivide    */
  NULL,                                /* fn_mrdivide   */
  NULL,                                /* fn_mldivide   */
  NULL,                                /* fn_power      */
  NULL,                                /* fn_mpower     */
  NULL,                                /* fn_lt         */
  NULL,                                /* fn_gt         */
  NULL,                                /* fn_le         */
  NULL,                                /* fn_ge         */
  NULL,                                /* fn_ne         */
  NULL,                                /* fn_eq         */
  NULL,                                /* fn_and        */
  NULL,                                /* fn_or         */
  NULL,                                /* fn_mand       */
  NULL,                                /* fn_mor        */
  NULL,                                /* fn_not        */
  NULL,                                /* fn_colon      */
  NULL,                                /* fn_ctranspose */
  NULL,                                /* fn_transpose  */
  (obj_variadic) string_horzcat,       /* fn_horzcat    */
  (obj_variadic) string_vertcat,       /* fn_vertcat    */
  NULL,                                /* fn_subsref    */
  NULL,                                /* fn_subsasgn   */
  NULL                                 /* fn_subsindex  */
};

