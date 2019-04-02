#include "cmd.h"
#include "restconf-method.h"
#include "vector.h"

int free_uci_write_list(uci_write_pair **list) {
  for (size_t i = 0; i < vector_size(list); i++) {
    uci_write_pair *cmd = list[i];
    free(cmd->value);
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

int write_uci_write_list(uci_write_pair **write_list) {
  struct path_section_pair *section_list = NULL;
  for (size_t i = 0; i < vector_size(write_list); i++) {
    int failed = 1;
    char local_path_string[512];
    uci_write_pair *cmd = write_list[i];
    if (cmd->path.where && cmd->path.section_type) {
      int section_exists = section_already_created(
          section_list, cmd->path.section_type, cmd->path.index);
      if (!section_exists) {
        uci_add_section_anon(cmd->path.package, cmd->path.section_type);
        struct path_section_pair output = {
            .section_type = cmd->path.section_type, .index = cmd->path.index};
        vector_push_back(section_list, output);
      }
      combine_to_anonymous_path(&cmd->path, cmd->path.index, local_path_string,
                                sizeof(local_path_string));
    } else {
      uci_add_section_named(cmd->path.package, "container", cmd->path.section);
      combine_to_path(&cmd->path, local_path_string, sizeof(local_path_string));
    }
    switch (cmd->type) {
      case list:
        failed = uci_write_list(local_path_string, cmd->value);
        break;
      case option:
        failed = uci_write_option(local_path_string, cmd->value);
        break;
      case container:
        failed = uci_add_section_named(cmd->path.package, "container",
                                       cmd->path.section);
        break;
    }
    if (failed) {
      // TODO: Discard delta
    }
  }
  return 0;
}