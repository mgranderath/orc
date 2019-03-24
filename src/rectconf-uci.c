#include "restconf-uci.h"
#include "vector.h"
#include "http.h"
#include <uci.h>
#include <string.h>

int combine_to_path(struct uci_path *path, char *buffer, size_t size) {
    int retval = snprintf(buffer, size, "%s.%s.%s", path->package, path->section, path->option);
    if (retval < 1) {
        return 1;
    } else {
        return 0;
    }
}

int uci_read_option(struct uci_path *path, char *buffer) {
    struct uci_ptr ptr;
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx) {
        return 1;
    }

    char buf[256];
    combine_to_path(path, buf, 256);

    int UCI_LOOKUP_COMPLETE = (1 << 1);

    if ((uci_lookup_ptr(ctx, &ptr, buf, true) != UCI_OK) || (ptr.o == NULL || ptr.o->v.string == NULL)) {
        uci_free_context(ctx);
        return 1;
    }

    if (ptr.flags & UCI_LOOKUP_COMPLETE) {
        strcpy(buffer, ptr.o->v.string);
    }
    uci_free_context(ctx);
    return 0;
}

int uci_read_list(struct uci_path *path, char** ret) {
    struct uci_ptr ptr;
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx) {
        return 1;
    }

    char buf[256];
    combine_to_path(path, buf, 256);

    int UCI_LOOKUP_COMPLETE = (1 << 1);

    if ((uci_lookup_ptr(ctx, &ptr, buf, true) != UCI_OK) || (ptr.o == NULL || ptr.o->v.string == NULL)) {
        uci_free_context(ctx);
        return 1;
    }

    if (ptr.flags & UCI_LOOKUP_COMPLETE) {
        struct uci_element *e;
        uci_foreach_element(&ptr.o->v.list, e) {
            vector_push_back(ret, str_dup(e->name));
        }
    }
    uci_free_context(ctx);
    return 0;
}