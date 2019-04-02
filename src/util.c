#include "util.h"
#include <stddef.h>
#include <string.h>
#include "http.h"

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
}