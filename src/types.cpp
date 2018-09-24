/*  Copyright (C) 2014-2018 FastoGT. All right reserved.
    This file is part of iptv_cloud.
    iptv_cloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    iptv_cloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with iptv_cloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "types.h"

#include <array>
#include <limits>

#include <common/macros.h>

namespace iptv_cloud {
namespace {
const std::array<const char *, NUM_STREAM_TYPES> stream_types = {
    {"screen", "relay", "encoding", "timeshift", "catchup"}};
}

const stream_id_t invalid_stream_id = std::numeric_limits<stream_id_t>::max();
} // namespace iptv_cloud

namespace common {

std::string ConvertToString(iptv_cloud::StreamType stream_type) {
  if (stream_type < 0 || stream_type >= iptv_cloud::NUM_STREAM_TYPES) {
    DNOTREACHED();
    return std::string();
  }

  return iptv_cloud::stream_types[stream_type];
}

bool ConvertFromString(const std::string &from, iptv_cloud::StreamType *out) {
  if (!out) {
    return false;
  }

  for (size_t i = 0; i < iptv_cloud::stream_types.size(); ++i) {
    if (from == iptv_cloud::stream_types[i]) {
      *out = static_cast<iptv_cloud::StreamType>(i);
      return true;
    }
  }

  NOTREACHED() << "Unknown stream type: " << from;
  return false;
}

} // namespace common
