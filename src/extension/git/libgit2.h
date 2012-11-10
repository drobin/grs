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

#ifndef LIBGIT2_H
#define LIBGIT2_H

#include <libgrs/binbuf.h>

/**
 * Implementation of the reference_discovery_cb-callback using the
 * libgit2-library.
 */
int libgit2_reference_discovery_cb(const char* repository, binbuf_t refs);

#endif  /* LIBGIT2_H */