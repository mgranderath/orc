#include <stdio.h>
#include <stdlib.h>
#include <json.h>
#include <string.h>
#include <uci.h>
#include "vector.h"
#include "cgi.h"
#include "http.h"
#include "yang.h"
#include "restconf-method.h"

#define IETF_YANG_VERSION "2016-06-21"

static int api_root(struct cgi_context *cgi) {
  content_type_json();
  headers_end();
  char *api = "{\n"
              "  \"ietf-restconf:restconf\" : {\n"
              "    \"data\" : {},\n"
              "    \"operations\" : {},\n"
              "    \"yang-library-version\" : \"" IETF_YANG_VERSION "\"\n"
              "  }\n"
              "}";
  struct json_object *jobj = json_tokener_parse(api);
  printf("%s\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
  json_object_put(jobj);
  return 0;
}

static int data_root(struct cgi_context *cgi, char** pathvec) {
  int retval = 1;

  if (pathvec[1] == NULL) {
    // root
    if (strcmp(cgi->method, "OPTIONS") == 0) {
      content_type_json();
      printf("Allow: OPTIONS,HEAD,GET,POST,PUT,DELETE\n");
      headers_end();
      goto done;
    } else if (strcmp(cgi->method, "HEAD") == 0) {

    }
    goto done;
  }

  if (strcmp(cgi->method, "GET") == 0) {
    retval = data_get(cgi, pathvec);
    goto done;
  }

  retval = 0;
  done:
  return retval;
}

static int operations_root(struct cgi_context *cgi) {
  content_type_json();
  headers_end();

  printf("Operations root\n");
  return 0;
}

static int yang_library_version(struct cgi_context *cgi) {
  content_type_json();
  headers_end();

  char *yang_version = "{ \"yang-library-version\": \"" IETF_YANG_VERSION "\" }";
  struct json_object *jobj = json_tokener_parse(yang_version);
  printf("%s\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
  json_object_put(jobj);
  return 0;
}

int main(void) {
  int retval = 1;
  char *path_modify = NULL;
  char **vec = NULL;

  struct cgi_context *ctx = cgi_context_init();
  if (!ctx) {
    internal_server_error(ctx);
    goto done;
  }

  if (ctx->media_accept && (strcmp(ctx->media_accept, "application/yang-data+json") != 0 && strcmp(ctx->media_accept, "*/*") != 0)) {
    notacceptable(ctx);
    goto done;
  }
  if (ctx->content_type && strcmp(ctx->content_type, "application/yang-data+json") != 0) {
    notacceptable(ctx);
    goto done;
  }

  if (ctx->path == NULL) {
    retval = api_root(ctx);
    goto done;
  }

  path_modify = str_dup(ctx->path);
  vec = path2vec(path_modify, "/");

  if (strcmp(vec[0], "data") == 0) {
    retval = data_root(ctx, vec);
    goto done;
  } else if (strcmp(vec[0], "operations") == 0) {
    retval = operations_root(ctx);
    goto done;
  } else if (strcmp(vec[0], "yang-library-version") == 0) {
    retval = yang_library_version(ctx);
    goto done;
  } else {
    retval = notfound(ctx);
    goto done;
  }

  retval = 0;
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
