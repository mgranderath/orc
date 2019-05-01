#include "util.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "http.h"
#include "vector.h"

/**
 * @brief split a string into two parts at delimiter
 * @param split the string to be split
 * @param first the first part of the string
 * @param second the second part of the string
 * @param split_char the delimiter character
 * @return 0 if success else 1
 */
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

/**
 * @brief duplicates a string up to size
 * Taken from original strndup source
 * @param s the string to be duplicated
 * @param n the size of the new string
 * @return the duplicated string
 */
char *strn_dup(const char *s, size_t n) {
  size_t len = strnlen(s, n);
  char *new = (char *)malloc(len + 1);
  if (new == NULL) {
    return NULL;
  }
  new[len] = '\0';
  return (char *)memcpy(new, s, len);
}

/**
 * @brief checks if string is in vector
 * @param vec the vector to be checked
 * @param value the string value
 * @return 1 if found else 0
 */
int is_in_vector(char **vec, char *value) {
  for (size_t i = 0; i < vector_size(vec); i++) {
    char *item = vec[i];
    if (strcmp(item, value) == 0) {
      return 1;
    }
  }
  return 0;
}