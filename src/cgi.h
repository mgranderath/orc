#ifndef _CGI_H
#define _CGI_H

/**
 * A structure to combine the individual env variables
 */
struct CgiContext {
  char *path;
  char *path_full;
  const char *query;
  const char *method;
  const char *media_accept;
  const char *content_type;
  int https;
  const char *host;
};

struct CgiContext *cgi_context_init();
void cgi_context_free(struct CgiContext *ctx);
char *get_content();

#endif