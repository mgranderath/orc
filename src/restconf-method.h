#ifndef _METHOD_H
#define _METHOD_H

#include "cgi.h"
#include "error.h"
#include "uci/methods.h"

typedef char** rvec;

struct path_section_pair {
  char* section_type;
  int index;
};

int data_get(struct cgi_context* cgi, char** pathvec);
int data_post(struct cgi_context* cgi, char** pathvec, int root);
int data_delete(struct cgi_context* cgi, char** pathvec, int root);
int data_put(struct cgi_context* cgi, char** pathvec, int root);

struct json_object* build_recursive(struct json_object* jobj,
                                    struct uci_path* path, error* err,
                                    int root);
struct uci_path* extract_paths(struct json_object* node, struct uci_path* uci,
                               error* err);

#endif