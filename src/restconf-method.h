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

int data_get(struct CgiContext* cgi, char** pathvec);
int data_post(struct CgiContext* cgi, char** pathvec, int root);
int data_delete(struct CgiContext* cgi, char** pathvec, int root);
int data_put(struct CgiContext* cgi, char** pathvec, int root);

struct json_object* build_recursive(struct json_object* jobj,
                                    struct UciPath* path, error* err, int root);
struct UciPath* extract_paths(struct json_object* node, struct UciPath* uci,
                              error* err);

#endif