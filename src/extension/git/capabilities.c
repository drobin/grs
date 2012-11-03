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

#include <ctype.h>
#include <string.h>

#include <libgrs/log.h>
#include "capabilities.h"

struct capabilitiy_map
{
  const char* from;
  enum capabilities to;
};

const static struct capabilitiy_map capability_mapping[] = {
  { "multi_ack", multi_ack },
  { "multi_ack_detailed", multi_ack_detailed },
  { "thin-pack", thin_pack},
  { "side-band", side_band},
  { "side-band-64k", side_band_64k},
  { "ofs-delta", ofs_delta},
  { "shallow", shallow},
  { "no-progress", no_progress},
  { "include-tag",include_tag},
  { "report-status", report_status},
  { "delete-refs", delete_refs},
  { NULL, 0 }
};

static int next_capability(const char* str, size_t* start, size_t* end) {
  int idx;

  // Find the start of the next capability
  idx = *start;
  while (isspace(str[idx])) {
    if (idx == *end) {
      // End of string reached, no capability
      return 0;
    } else {
      idx++;
    }
  }

  *start = idx;

  // Find end of next capability
  while (isalpha(str[idx]) || isdigit(str[idx]) ||
         str[idx] == '-' || str[idx] == '_') {
    if (idx == *end) {
      // End of string reached
      *end = idx;
      return 1;
    } else {
      idx++;
    }
  }

  *end = idx - 1;

  return 1;
}

int capabilities_parse(const char* str, size_t len) {
  size_t start, end;
  int result;

  if (str == NULL) {
    return -1;
  }

  if (len == 0) {
    // No data available
    return 0;
  }

  result = 0;
  start = 0;
  end = len - 1;

  while (next_capability(str, &start, &end)) {
    int idx;
    int map_to = 0;

    for (idx = 0; capability_mapping[idx].from != NULL; idx++ ) {
      const size_t cmplen = strlen(capability_mapping[idx].from);

      if (end - start + 1 != cmplen) {
        // Wrong size
        continue;
      }

      if (strncmp(capability_mapping[idx].from, str + start, cmplen) == 0) {
        map_to = capability_mapping[idx].to;
        log_debug("capability: %s", capability_mapping[idx].from);
        break;
      }
    }

    if (map_to == 0) {
      char s[end - start + 2];
      strlcpy(s, str + start, sizeof(s));

      log_err("capability: no mapping for '%s'", s);

      return -1;
    }

    result |= map_to;
    start = end + 1;
    end = len - 1;

    if (start >= len) {
      break;
    }
  }

  return result;
}
