#include "restconf.h"
#include <json.h>
#include <stdio.h>
#include <string.h>
#include "cgi.h"
#include "generated/yang.h"
#include "http.h"
#include "restconf-json.h"
#include "restconf-method.h"
#include "util.h"
#include "vector.h"

/**
 * @brief the api root method
 * @param cgi the cgi context
 */
static int api_root(struct cgi_context *cgi) {
  char api[] =
      "{\n"
      "  \"ietf-restconf:restconf\" : {\n"
      "    \"data\" : {},\n"
      "    \"operations\" : {},\n"
      "    \"yang-library-version\" : \"" IETF_YANG_VERSION
      "\"\n"
      "  }\n"
      "}";

  content_type_json();
  headers_end();
  printf("%s\n", api);
  return 0;
}

/**
 * @brief the data root method
 * @param cgi the cgi context
 * @param pathvec the path vector
 */
static int data_root(struct cgi_context *cgi, char **pathvec) {
  int retval = 1;

  if (pathvec[1] == NULL) {
    // root
    if (strcmp(cgi->method, "OPTIONS") == 0) {
      content_type_json();
      printf("Allow: OPTIONS,HEAD,GET,POST,PUT,DELETE\n");
      headers_end();
      goto done;
    } else if (strcmp(cgi->method, "HEAD") == 0) {
    } else if (strcmp(cgi->method, "POST") == 0) {
      retval = data_post(cgi, pathvec, 1);
    } else if (strcmp(cgi->method, "PUT") == 0) {
      retval = data_put(cgi, pathvec, 1);
    } else {
      retval = notfound(cgi);
    }
    goto done;
  }

  if (strcmp(cgi->method, "GET") == 0) {
    retval = data_get(cgi, pathvec);
  } else if (strcmp(cgi->method, "POST") == 0) {
    retval = data_post(cgi, pathvec, 0);
  } else if (strcmp(cgi->method, "DELETE") == 0) {
    retval = data_delete(cgi, pathvec, 0);
  } else if (strcmp(cgi->method, "PUT") == 0) {
    retval = data_put(cgi, pathvec, 0);
  } else {
    retval = notfound(cgi);
  }

done:
  return retval;
}

/**
 * @brief the operations root
 * @param cgi the cgi context
 */
static int operations_root(struct cgi_context *cgi) {
  content_type_json();
  headers_end();

  printf("Operations root\n");
  return 0;
}

/**
 * @brief the yang library version method
 * @param cgi the cgi context
 */
static int yang_library_version(struct cgi_context *cgi) {
  char yang_version[] =
      "{ \"yang-library-version\": \"" IETF_YANG_VERSION "\" }";
  struct json_object *retval = NULL;

  content_type_json();
  headers_end();
  retval = json_tokener_parse(yang_version);
  json_pretty_print(retval);
  json_object_put(retval);
  return 0;
}

int main(void) {
  int retval = 1;
  char *path_modify = NULL;
  char **vec = NULL;
  struct cgi_context *ctx = NULL;

  ctx = cgi_context_init();
  if (!ctx) {
    internal_server_error(ctx);
    goto done;
  }

  if (ctx->media_accept &&
      (strcmp(ctx->media_accept, "application/yang-data+json") != 0 &&
       strcmp(ctx->media_accept, "*/*") != 0)) {
    notacceptable(ctx);
    goto done;
  }
  if (ctx->content_type &&
      strcmp(ctx->content_type, "application/yang-data+json") != 0) {
    notacceptable(ctx);
    goto done;
  }

  if (ctx->path == NULL || strlen(ctx->path) <= 1) {
    retval = api_root(ctx);
    goto done;
  }

  path_modify = str_dup(ctx->path);

  if (!(vec = path2vec(path_modify, "/"))) {
    retval = notfound(ctx);
    goto done;
  }

  if (strcmp(vec[0], "data") == 0) {
    retval = data_root(ctx, vec);
  } else if (strcmp(vec[0], "operations") == 0) {
    retval = operations_root(ctx);
  } else if (strcmp(vec[0], "yang-library-version") == 0) {
    retval = yang_library_version(ctx);
  } else {
    retval = notfound(ctx);
  }

done:
  if (vec) {
    vector_free(vec);
  }
  cgi_context_free(ctx);
  if (path_modify) {
    free(path_modify);
  }
  return retval;
}
