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

#ifndef _SESSION_H
#define _SESSION_H

#include <libssh/libssh.h>
#include <libssh/server.h>

struct _session {
  /**
   * The SSH-session.
   */
  ssh_session session;

  /**
   * The SSH-channel.
   */
  ssh_channel channel;

  /**
   * State of the session.
   */
  enum session_state state;
};

#endif  /* _SESSION_H */
