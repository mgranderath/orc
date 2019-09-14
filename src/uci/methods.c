#include "methods.h"
#include <string.h>
#include <uci.h>
#include "http.h"
#include "uci-util.h"
#include "util.h"
#include "vector.h"

/**
 * reads a uci option into a buffer by path
 * @param path the path to be used
 * @param buffer the buffer the output should be copied to
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int uci_read_option(char *path, char *buffer, size_t size) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }

  unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

  if ((uci_lookup_ptr(ctx, &ptr, path, true) != UCI_OK) ||
      (ptr.o == NULL || ptr.o->v.string == NULL)) {
    uci_free_context(ctx);
    return 1;
  }

  if (ptr.flags & UCI_LOOKUP_COMPLETE) {
    strncpy(buffer, ptr.o->v.string, size);
  }
  uci_free_context(ctx);
  return 0;
}

/**
 * reads a uci option into a buffer by path
 * @param path the path to be used
 * @param ret the initialized vector the output should be copied to
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
char **uci_read_list(char *path) {
  struct uci_ptr ptr;
  char **ret = NULL;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return NULL;
  }

  unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

  if ((uci_lookup_ptr(ctx, &ptr, path, true) != UCI_OK) ||
      (ptr.o == NULL || ptr.o->v.string == NULL)) {
    uci_free_context(ctx);
    return NULL;
  }

  if (ptr.flags & UCI_LOOKUP_COMPLETE) {
    struct uci_element *e;
    uci_foreach_element(&ptr.o->v.list, e) {
      vector_push_back(ret, str_dup(e->name));
    }
  }
  uci_free_context(ctx);
  return ret;
}

int uci_path_exists(char *path) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 0;
  }
  char *path_dup = str_dup(path);
  if (!path_dup) {
    return 0;
  }

  unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

  if ((uci_lookup_ptr(ctx, &ptr, path_dup, true) != UCI_OK) ||
      (ptr.s == NULL && ptr.o == NULL)) {
    uci_free_context(ctx);
    free(path_dup);
    return 0;
  }

  uci_free_context(ctx);
  free(path_dup);

  return ptr.flags & UCI_LOOKUP_COMPLETE;
}

int uci_index_where(struct UciWhere *where) {
  int index = 0;
  char *package = str_dup(where->path->package);
  char *section_type = str_dup(where->path->section_type);
  struct UciPath path = {.package = package,
                         .section_type = section_type,
                         .section = "",
                         .option = "",
                         .index = 0,
                         .where = 0};
  while (1) {
    int notfound = 0;
    for (int i = 0; i < where->key_value_length; i++) {
      char path_string[512];
      char buf[512];
      path.option = (char *)where->key_value[i].key;
      combine_to_anonymous_path(&path, index, path_string, sizeof(path_string));
      if (uci_read_option(path_string, buf, sizeof(buf))) {
        index = -1;
        break;
      }
      if (strcmp(buf, where->key_value[i].str) != 0) {
        notfound = 1;
        break;
      }
    }
    if (!notfound) {
      break;
    }
    index++;
  }
  free(package);
  free(section_type);
  return index;
}

int uci_write_option(char *path, const char *value) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }

  if (uci_lookup_ptr(ctx, &ptr, path, true) != UCI_OK) {
    uci_free_context(ctx);
    return 1;
  }

  ptr.value = value;
  if ((uci_set(ctx, &ptr) != UCI_OK) ||
      (ptr.o == NULL || ptr.o->v.string == NULL)) {
    uci_free_context(ctx);
    return 1;
  }
  uci_commit(ctx, &ptr.p, false);

  uci_free_context(ctx);
  return 0;
}

int uci_write_list(char *path, const char *value) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }

  if (uci_lookup_ptr(ctx, &ptr, path, true) != UCI_OK) {
    uci_free_context(ctx);
    return 1;
  }

  ptr.value = value;
  if ((uci_add_list(ctx, &ptr) != UCI_OK) ||
      (ptr.o == NULL || ptr.o->v.string == NULL)) {
    uci_free_context(ctx);
    return 1;
  }
  uci_commit(ctx, &ptr.p, false);

  uci_free_context(ctx);
  return 0;
}

int uci_list_length(struct UciPath *path) {
  int index = 0;
  if (!path->package || !path->section_type) {
    return -1;
  }
  char *package = str_dup(path->package);
  char *section_type = str_dup(path->section_type);
  if (!package || !section_type) {
    return -1;
  }
  struct UciPath cloned_path = {.section = "",
                                .package = package,
                                .section_type = section_type,
                                .index = 0,
                                .where = 0,
                                .option = ""};

  while (1) {
    char path_string[512];
    combine_to_anonymous_path(&cloned_path, index, path_string,
                              sizeof(path_string));
    struct uci_ptr ptr;
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx) {
      return -1;
    }

    unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

    if ((uci_lookup_ptr(ctx, &ptr, path_string, true) != UCI_OK)) {
      uci_free_context(ctx);
      break;
    }

    if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
      uci_free_context(ctx);
      break;
    }
    uci_free_context(ctx);
    index++;
  }

  free(package);
  free(section_type);
  return index;
}

struct uci_section *uci_add_section_anon(char *package_name, char *type) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return NULL;
  }
  if ((uci_lookup_ptr(ctx, &ptr, package_name, true) != UCI_OK)) {
    uci_free_context(ctx);
    return NULL;
  }
  struct uci_section *section = NULL;
  uci_add_section(ctx, ptr.p, type, &section);
  uci_commit(ctx, &ptr.p, false);

  uci_free_context(ctx);
  return section;
}

int uci_add_section_named(char *package_name, const char *type, char *name) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }
  char path_string[512];
  snprintf(path_string, sizeof(path_string), "%s.%s", package_name, name);
  if ((uci_lookup_ptr(ctx, &ptr, path_string, true) != UCI_OK)) {
    uci_free_context(ctx);
    return 1;
  }
  ptr.value = type;
  uci_set(ctx, &ptr);
  uci_commit(ctx, &ptr.p, false);

  uci_free_context(ctx);
  return 0;
}

int uci_revert_package(char *package) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }

  unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

  if ((uci_lookup_ptr(ctx, &ptr, package, true) != UCI_OK) || (ptr.p == NULL)) {
    uci_free_context(ctx);
    return 1;
  }

  if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
    uci_free_context(ctx);
    return 1;
  }

  if (uci_revert(ctx, &ptr)) {
    uci_free_context(ctx);
    return 1;
  }

  uci_free_context(ctx);
  return 0;
}

int uci_delete_path(char *path, int commit) {
  struct uci_ptr ptr;
  char *dup_path = NULL;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }

  unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

  if (!(dup_path = str_dup(path))) {
    return 1;
  }
  if ((uci_lookup_ptr(ctx, &ptr, dup_path, true) != UCI_OK) ||
      (ptr.o == NULL && ptr.s == NULL)) {
    free(dup_path);
    uci_free_context(ctx);
    return -1;
  }

  if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
    free(dup_path);
    uci_free_context(ctx);
    return -1;
  }

  if (uci_delete(ctx, &ptr)) {
    free(dup_path);
    uci_free_context(ctx);
    return 1;
  }

  if (commit) {
    uci_commit(ctx, &ptr.p, false);
  } else {
    uci_save(ctx, ptr.p);
  }
  free(dup_path);
  uci_free_context(ctx);
  return 0;
}

int uci_commit_package(char *package) {
  struct uci_ptr ptr;
  struct uci_context *ctx = uci_alloc_context();
  if (!ctx) {
    return 1;
  }

  unsigned int UCI_LOOKUP_COMPLETE = (1u << 1u);

  if ((uci_lookup_ptr(ctx, &ptr, package, true) != UCI_OK) || (ptr.p == NULL)) {
    uci_free_context(ctx);
    return 1;
  }

  if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
    uci_free_context(ctx);
    return 1;
  }

  if (uci_commit(ctx, &ptr.p, false)) {
    uci_free_context(ctx);
    return 1;
  }

  uci_free_context(ctx);
  return 0;
}