#ifndef _CGI_H
#define _CGI_H

/**
 * A structure to combine the individual env variables
 */
struct cgi_context {
  char *path;
  char *path_full;
  char *query;
  char *method;
  char *media_accept;
  char *content_type;
  int https;
  char *host;
};

struct cgi_context *cgi_context_init();
void cgi_context_free(struct cgi_context *ctx);
char *get_content();

#endif