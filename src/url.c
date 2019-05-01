#include "url.h"
#include <ctype.h>
#include <string.h>
#include "vector.h"

/**
 * @brief convert comma separated list into vector
 * @param list the comma separated list
 * @return the vector of seperated items
 */
char **clist_to_vec(char *list) {
  char **vec = NULL;
  char *next;
  char *curr = list;
  while ((next = strchr(curr, ',')) != NULL) {
    *next = '\0';
    vector_push_back(vec, curr);
    curr = next + 1;
  }
  vector_push_back(vec, curr);
  return vec;
}

/**
 * @brief Decodes a URL
 * Taken from https://stackoverflow.com/a/14530993
 * @param dst the output string
 * @param src the string to be decoded
 */
void urldecode(char *dst, const char *src) {
  char a, b;
  while (*src) {
    if ((*src == '%') && ((a = src[1]) && (b = src[2])) &&
        (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a') a -= 'a' - 'A';
      if (a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if (b >= 'a') b -= 'a' - 'A';
      if (b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';
      *dst++ = (char)16 * a + b;
      src += 3;
    } else if (*src == '+') {
      *dst++ = ' ';
      src++;
    } else {
      *dst++ = *src++;
    }
  }
  *dst++ = '\0';
}