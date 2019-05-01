#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"
#include "restconf.h"

/**
 * Create the cgi_context from environment variables
 * @return cgi_context
 */
struct cgi_context *cgi_context_init() {
  struct cgi_context *ctx;

  ctx = (struct cgi_context *)malloc(sizeof(struct cgi_context));
  if (!ctx) {
    return NULL;
  }

  char *path = getenv("REQUEST_URI");
  path = path + strlen(ROOT);
  char *query = getenv("QUERY_STRING");
  char *method = getenv("REQUEST_METHOD");
  char *media_accept = getenv("HTTP_ACCEPT");
  char *content_type = getenv("CONTENT_TYPE");
  char *host = getenv("HTTP_HOST");
  if (!method) {
    return NULL;
  }

  ctx->path = path;
  ctx->path_full = getenv("REQUEST_URI");
  ctx->query = query;
  ctx->method = method;
  ctx->media_accept = media_accept;
  ctx->content_type = content_type;
  ctx->https = getenv("HTTPS") ? 1 : 0;
  ctx->host = host;

  return ctx;
}

/**
 * Frees the cgi_context
 * @param ctx the cgi_context to be freed
 */
void cgi_context_free(struct cgi_context *ctx) { free(ctx); }

/**
 * Get the content passed in stdin
 * @return the allocated content
 */
char *get_content() {
  char *post_data;
  char *length_str;
  char *trailing;
  unsigned long length;

  length_str = getenv("CONTENT_LENGTH");
  if (!length_str || !*length_str) return NULL;

  length = strtoul(length_str, &trailing, 10);
  if (*trailing != '\0' || !length || length > 1024UL * 1024UL) return NULL;

  post_data = (char *)malloc(length + 1);
  if (!post_data) return NULL;

  if (fread(post_data, sizeof(char), length, stdin) == length) {
    post_data[length] = '\0';
  }

  return post_data;
}