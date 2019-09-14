#ifndef _HTTP_H
#define _HTTP_H

#include <stddef.h>
#include "cgi.h"

int bad_request(struct CgiContext *ctx);
int not_found(struct CgiContext *ctx);
int forbidden(struct CgiContext *ctx);
int not_acceptable(struct CgiContext *ctx);
int internal_server_error(struct CgiContext *ctx);
int not_implemented(struct CgiContext *ctx);

void content_type_json();
void headers_end();

char **path2vec(char *path, char *identifier);

int is_GET(const char* method);
int is_OPTIONS(const char* method);
int is_HEAD(const char* method);
int is_POST(const char* method);
int is_DELETE(const char* method);
int is_PUT(const char* method);

#endif