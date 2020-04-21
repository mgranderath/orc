#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"
#include "restconf.h"

#define MAX_BUFFER_SIZE 1024

/**
 * Create the cgi_context from environment variables
 * @return cgi_context
 */
struct CgiContext *cgi_context_init() {
  struct CgiContext *ctx;

  ctx = (struct CgiContext *)malloc(sizeof(struct CgiContext));
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
void cgi_context_free(struct CgiContext *ctx) { free(ctx); }

/**
 * Get the content passed in stdin
 * @return the allocated content
 */
char *get_content() {
  char *post_data;
  char *length_str;
  char *trailing;
  char *file_path;
  unsigned long length;

  length_str = getenv("CONTENT_LENGTH");
  if (!length_str || !*length_str) {
    file_path = getenv("CONTENT_FILE_PATH");
    if (!file_path || !*file_path) {
      return NULL;
    }

    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
      return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    post_data = (char *) malloc(sizeof(char) * size);
    if (!post_data) {
      fclose(fp);
      return NULL;
    }

    if (fread(post_data, sizeof(char), size, fp) == size) {
      post_data[size] = '\0';
    }
    fclose(fp);

  } else {
    length = strtoul(length_str, &trailing, 10);
    if (*trailing != '\0' || !length || length > 1024UL * 1024UL) return NULL;

    post_data = (char *)malloc(length + 1);
    if (!post_data) return NULL;

    if (fread(post_data, sizeof(char), length, stdin) == length) {
      post_data[length] = '\0';
    }
  }

  return post_data;
}