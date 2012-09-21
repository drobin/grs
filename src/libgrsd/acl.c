/*******************************************************************************
 *
 * This file is part of grs.
 *
 * grs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * grs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with grs.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "acl.h"

struct _acl {
  acl_node_t root;
};

struct _acl_node {
  char* name;
};

static struct _acl_node* acl_node_get_or_create(const char* name) {
  struct _acl_node* node;

  if ((node = malloc(sizeof(struct _acl_node))) == NULL) {
    return NULL;
  }

  memset(node, 0, sizeof(struct _acl_node));

  if (name != NULL) {
    node->name = strdup(name);
  }

  return node;
}

static int acl_node_destroy(struct _acl_node* node) {
  free(node->name);
  free(node);

  return 0;
}

acl_t acl_init() {
  struct _acl* acl;

  if ((acl = malloc(sizeof(struct _acl))) == NULL) {
    return NULL;
  }

  memset(acl, 0, sizeof(struct _acl));

  return acl;
}

int acl_destroy(acl_t acl) {
  if (acl == NULL) {
    return -1;
  }

  if (acl->root != NULL) {
    acl_node_destroy(acl->root);
  }

  free(acl);

  return 0;
}

acl_node_t acl_get_node(acl_t acl, const char** path) {
  if (acl == NULL || path == NULL) {
    return NULL;
  }

  if (acl->root == NULL) {
    // First, create the root-node
    if ((acl->root = acl_node_get_or_create(NULL)) == NULL) {
      return NULL;
    }
  }

  if (path[0] == NULL) {
    return acl->root;
  } else {
    return NULL;
  }
}
