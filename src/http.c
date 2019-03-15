#include <string.h>
#include <stdio.h>
#include "http.h"
#include "vector.h"

char *str_dup(const char *c) {
  char *dup = malloc(strlen(c) + 1);

  if (dup != NULL)
      strcpy(dup, c);

  return dup;
}

/**
 * @brief splits path into fields
 *
 * @param path the path to be split
 * @param identifier the identifier to split
 * @return char** vector of strings
 */
char** path2vec(char* path, char* identifier) {
  char **vec = NULL;
  char *token = strtok(path, identifier);
  while (token != NULL) {
    vector_push_back(vec, token);
    token = strtok(NULL, identifier);
  }
  return vec;
}

/**
 * @brief HTTP error 400
 *
 * @param ctx CGI context
 * @return int success value
 */
int badrequest(struct cgi_context *ctx) {
  printf("Status: 400 Bad Request\r\n");
  printf("Content-Type: text/html\r\n\r\n");

  printf("<h1>Bad Request</h1>\n");
  printf("Bad request\n");
  printf("The requested URL is badly formed.\n");
  return 0;
}

/**
 * @brief HTTP error 404
 *
 * @param ctx CGI context
 * @return int success value
 */
int notfound(struct cgi_context *ctx) {
  printf("Status: 404 Not Found\r\n");
  printf("Content-Type: text/html\r\n\r\n");

  printf("<h1>Not Found</h1>\n");
  printf("Not Found\n");
  printf("The requested URL %s was not found on this server.\n", ctx->path);
  return 0;
}

/**
 * @brief HTTP error 403
 *
 * @param ctx CGI context
 * @return int success value
 */
int forbidden(struct cgi_context *ctx) {
  printf("Status: 403 Forbidden\r\n");
  printf("Content-Type: text/html\r\n\r\n");

  printf("<h1>Forbidden</h1>\n");
  printf("The requested URL %s was forbidden.\n", ctx->path);;
  return 0;
}

/**
 * @brief HTTP error 406
 *
 * @param ctx CGI context
 * @return int success value
 */
int notacceptable(struct cgi_context *ctx) {
  printf("Status: 406 Not Acceptable\r\n");
  printf("Content-Type: text/html\r\n\r\n");

  printf("<h1>Not Acceptable</h1>\n");
  printf("Not Acceptable\n");
  return 0;
}

/**
 * @brief HTTP error 500
 *
 * @param ctx CGI context
 * @return int success value
 */
int internal_server_error(struct cgi_context *ctx) {
  printf("Status: 500 Internal Server Error\r\n");
  printf("Content-Type: text/html\r\n\r\n");

  printf("<h1>Internal server error</h1>\n");
  return 0;
}

void content_type_json() {
  printf("Content-Type: application/yang-data+json;charset=utf-8;\n");
}

void headers_end() {
  printf("\n\n");
}