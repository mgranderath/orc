#include "cmd.h"
#include <util.h>
#include "restconf-method.h"
#include "uci-util.h"
#include "vector.h"

struct UciWritePair *initialize_uci_write_pair(struct UciPath *path,
                                               char *value,
                                               enum uci_object_type type) {
  UciWritePair *output = malloc(sizeof(UciWritePair));
  if (!output) {
    return NULL;
  }
  output->path = *path;
  output->value = (char *)value;
  output->type = type;
  return output;
}

int free_uci_write_list(UciWritePair **list) {
  for (size_t i = 0; i < vector_size(list); i++) {
    free(list[i]);
  }
  vector_free(list);
  return 0;
}

static int section_already_created(struct path_section_pair *sections,
                                   char *section_type, int index) {
  for (size_t i = 0; i < vector_size(sections); i++) {
    struct path_section_pair curr = sections[i];
    if (strcmp(curr.section_type, section_type) == 0 && curr.index == index) {
      return 1;
    }
  }
  return 0;
}

static int leaf_list_deleted(struct UciPath *path, struct UciPath *comp) {
  for (size_t i = 0; i < vector_size(path); i++) {
    struct UciPath curr = path[i];
    if (strcmp(curr.section_type, comp->section_type) == 0 &&
        strcmp(curr.section, comp->section) == 0 &&
        strcmp(curr.option, comp->option) == 0) {
      return 1;
    }
  }
  return 0;
}

int write_uci_write_list(UciWritePair **write_list) {
  struct path_section_pair *section_list = NULL;
  struct UciPath *path = NULL;
  for (size_t i = 0; i < vector_size(write_list); i++) {
    int failed = 1;
    char local_path_string[512];
    UciWritePair *cmd = write_list[i];
    if ((cmd->path.where && cmd->path.section == NULL) && cmd->path.section_type) {
      int section_exists = section_already_created(
          section_list, cmd->path.section_type, cmd->path.index);
      snprintf(local_path_string, sizeof(local_path_string), "%s.@%s[%d]",
               cmd->path.package, cmd->path.section_type, cmd->path.index);
      section_exists = section_exists || uci_path_exists(local_path_string);
      if (!section_exists) {
        uci_add_section_anon(cmd->path.package, cmd->path.section_type);
        struct path_section_pair output = {
            .section_type = cmd->path.section_type, .index = cmd->path.index};
        vector_push_back(section_list, output);
      }
      combine_to_anonymous_path(&cmd->path, cmd->path.index, local_path_string,
                                sizeof(local_path_string));
    } else {
      uci_add_section_named(cmd->path.package, cmd->path.section_type,
                            cmd->path.section);
      combine_to_path(&cmd->path, local_path_string, sizeof(local_path_string));
    }
    if (cmd->type == list && !leaf_list_deleted(path, &cmd->path)) {
      char *duplicated_local_path_string = str_dup(local_path_string);
      if (!duplicated_local_path_string) {
        return 1;
      }
      uci_delete_path(duplicated_local_path_string, 1);
      free(duplicated_local_path_string);
      vector_push_back(path, cmd->path);
    }
    switch (cmd->type) {
      case list:
        failed = uci_write_list(local_path_string, cmd->value);
        break;
      case option:
        failed = uci_write_option(local_path_string, cmd->value);
        break;
      case container:
        failed = uci_add_section_named(
            cmd->path.package, cmd->path.section_type, cmd->path.section);
        break;
    }
    if (failed) {
      // TODO: Discard delta
    }
  }
  return 0;
}