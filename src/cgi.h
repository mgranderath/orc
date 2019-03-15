#ifndef _CGI_H
#define _CGI_H

struct cgi_context {
  char *path;
  char *query;
  char *method;
  char *media_accept;
  char *content_type;
};

struct cgi_context *cgi_context_init();
void cgi_context_free(struct cgi_context *ctx);
char* get_content();

#endif