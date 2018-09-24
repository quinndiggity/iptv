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
    along with iptv_cloud. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string> // for string

namespace iptv_cloud {

typedef uint64_t stream_id_t;
extern const stream_id_t invalid_stream_id;
typedef std::string channel_id_t;
typedef double volume_t;

enum StreamType {
  SCREEN = 0, // for inner use
  RELAY,
  ENCODING,
  TIMESHIFT_PLAYER,
  TIMESHIFT_RECORDER,
  CATCHUP,
  NUM_STREAM_TYPES
};

} // namespace iptv_cloud

namespace common {
std::string ConvertToString(iptv_cloud::StreamType type);
bool ConvertFromString(const std::string &from, iptv_cloud::StreamType *out);
} // namespace common
