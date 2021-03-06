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

#ifndef ACL_H
#define ACL_H

struct _acl;
struct _acl_node;

typedef struct _acl* acl_t;
typedef struct _acl_node* acl_node_t;

struct acl_node_value {
  /**
   * The flag describes whether you can access the node or not. This is the
   * simpliest way of access-controll.
   */
  int flag;
};

/**
 * Initializes the ACL-module.
 *
 * @return A handle to the ACL-module.
 */
acl_t acl_init();

/**
 * Destroys the handle to the ACL-module again.
 *
 * @param acl The handle to destroy
 * @return On success <code>0</code> is returned.
 */
int acl_destroy(acl_t acl);

/**
 * Tests whether you can access the given path.
 *
 * It walks througth the tree of acl-nodes starting from the end of path up to
 * the root-node and stops at the node, where some ACL-information are stored.
 * So you can overwrite the ACL from the more general nodes at or near the
 * root of the node-tree and the concrete nodes near the leave(s) of the tree.
 *
 * @param acl The ACL-handle
 * @param path The path to check.
 * @param len Number of path-elements in <code>path</code>. Note, that you need
 *            at least one element!
 * @return When you are allowed to access the given <code>path</code>, then
 *         <code>true</code> is returned.
 */
int acl_can(acl_t acl, const char** path, int len);

/**
 * Returns the root-node of the ACL-system.
 *
 * @param acl The ACL-handle
 * @return The root-node of the ACL-system
 */
acl_node_t acl_get_root_node(acl_t acl);

/**
 * Returns the node of the given path.
 *
 * The node is created, if it doesn't alredy exists.
 *
 * @param acl The ACL-handle
 * @param path The path of the node to be returned.
 * @param len Number of path-elements in <code>path</code>.
 * @return The node of the requested path.
 */
acl_node_t acl_get_node(acl_t acl, const char** path, int len);

/**
 * Tests whether you have a node for the given path.
 *
 * @param acl The ACL-handle
 * @param path The path to check
 * @param len Number of path-elements in <code>path</code>.
 * @return If you have such a node, then <code>true</code> is returned.
 */
int acl_has_node(acl_t acl, const char** path, int len);

/**
 * Return the parent of the given node.
 *
 * Note, for the root-node the parent is <code>NULL</code>!
 *
 * @param node The requested node
 * @return The parent of the node (if any)
 */
acl_node_t acl_node_get_parent(acl_node_t node);

/**
 * Returns the name of the given node.
 *
 * Usually this is a path-component. For the root-node the name is
 * <code>NULL</code>.
 *
 * @param node The requested node
 * @return The name of the node
 */
const char* acl_node_get_name(acl_node_t node);

/**
 * Returns the value of the node.
 *
 * The value can be <code>NULL</code>! In this case no value-structure is
 * assigned to the node. If you pass <code>true</code> to the
 * <code>create</code> argument, the the value-structure is created, if such a
 * structure does not already exists for the node.
 *
 * @param node The requested node
 * @param create If set to <code>true</code>, then the structure is created, if
 *               such a structure doesn't already exist for the node.
 * @return The value-structure of the node (if any)
 */
struct acl_node_value* acl_node_get_value(acl_node_t node, int create);

#endif  /* ACL_H */
