#include "util.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "http.h"
#include "vector.h"

int split_pair_by_char(char *split, char **first, char **second,
                       char split_char) {
  char *delimiter = NULL;
  size_t delimiter_index;
  delimiter = strchr(split, split_char);
  if (!delimiter) {
    // No : found in top
    return 1;
  }
  delimiter_index = (size_t)(delimiter - split);
  *first = strn_dup(split, delimiter_index);
  *second = str_dup(split + delimiter_index + 1);
  if (!*first || !*second) {
    return 1;
  }
  return 0;
}

/**
 * Duplicates a string
 * @param c the string to be duplicated
 * @return the duplicated string
 */
char *str_dup(const char *c) {
  char *dup = malloc(strlen(c) + 1);

  if (dup != NULL) strcpy(dup, c);

  return dup;
}

// Official strndup source
char *strn_dup(const char *s, size_t n) {
  size_t len = strnlen(s, n);
  char *new = (char *)malloc(len + 1);
  if (new == NULL) {
    return NULL;
  }
  new[len] = '\0';
  return (char *)memcpy(new, s, len);
}

int is_in_vector(char **vec, char *value) {
  for (size_t i = 0; i < vector_size(vec); i++) {
    char *item = vec[i];
    if (strcmp(item, value) == 0) {
      return 1;
    }
  }
  return 0;
}