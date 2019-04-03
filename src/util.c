#include "util.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "http.h"
#include "vector.h"

int split_module_top_level(char *split, char **module, char **top_level) {
  char *delimiter = NULL;
  size_t delimiter_index;
  delimiter = strchr(split, ':');
  if (!delimiter) {
    // No : found in top
    return 1;
  }
  delimiter_index = (size_t)(delimiter - split);
  *module = strn_dup(split, delimiter_index);
  *top_level = str_dup(split + delimiter_index + 1);
  if (!module || !top_level) {
    return 1;
  }
  return 0;
} /**
   * Duplicates a string
   * @param c the string to be duplicated
   * @return the duplicated string
   */
char *str_dup(const char *c) {
  char *dup = malloc(strlen(c) + 1);

  if (dup != NULL) strcpy(dup, c);

  return dup;
}  // Official strndup source
char *strn_dup(const char *s, size_t n) {
  size_t len = strnlen(s, n);
  char *new = (char *)malloc(len + 1);
  if (new == NULL) {
    return NULL;
  }
  new[len] = '\0';
  return (char *)memcpy(new, s, len);
}