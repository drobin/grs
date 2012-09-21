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
typedef struct _acl* acl_t;

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

#endif  /* ACL_H */
