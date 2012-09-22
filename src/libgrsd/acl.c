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
  struct _acl_node* first_child;
  struct _acl_node* parent;
  struct _acl_node* next;
  char* name;
  struct acl_node_value* value;
};

static struct _acl_node* acl_node_get_or_create(struct _acl_node* parent,
                                                const char* name) {
  struct _acl_node* node = NULL;

  if (parent != NULL) {
    node = parent->first_child;
  }

  // Search for the node in the list of all children
  while (node != NULL) {
    if (strcmp(node->name, name) == 0) {
      // Node found -> return
      return node;
    }

    node = node->next;
  }

  // The node was not found, create it now and assign the new node the the
  // list of children

  if ((node = malloc(sizeof(struct _acl_node))) == NULL) {
    return NULL;
  }

  memset(node, 0, sizeof(struct _acl_node));

  if (name != NULL) {
    node->name = strdup(name);
  }

  node->parent = parent;

  if (parent != NULL) {
    node->next = parent->first_child;
    parent->first_child = node;
  }

  return node;
}

static int acl_node_destroy(struct _acl_node* node) {
  struct _acl_node* child = node->first_child;

  while (child != NULL) {
    struct _acl_node* next = child->next;

    acl_node_destroy(child);
    child = next;
  }

  free(node->name);
  free(node->value);
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
  struct _acl_node* node;
  int i;

  if (acl == NULL || path == NULL) {
    return NULL;
  }

  if (acl->root == NULL) {
    // First, create the root-node
    if ((acl->root = acl_node_get_or_create(NULL, NULL)) == NULL) {
      return NULL;
    }
  }

  for (node = acl->root, i = 0; path[i] != NULL; i++) {
    node = acl_node_get_or_create(node, path[i]);
  }

  return node;
}

acl_node_t acl_node_get_parent(acl_node_t node) {
  if (node == NULL) {
    return NULL;
  }

  return node->parent;
}

const char* acl_node_get_name(acl_node_t node) {
  if (node == NULL) {
    return NULL;
  }

  return node->name;
}

struct acl_node_value* acl_node_get_value(acl_node_t node, int create) {
  if (node == NULL) {
    return NULL;
  }

  if (node->value == NULL && create) {
    node->value = malloc(sizeof(struct acl_node_value));
    memset(node->value, 0, sizeof(struct acl_node_value));
  }

  return node->value;
}
