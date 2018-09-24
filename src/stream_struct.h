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

#pragma once

#include <vector>

#include <common/macros.h>

#include "types.h"

namespace iptv_cloud {

enum StreamStatus {
  SNEW = 0,
  SNULL,
  SSTARTED,
  SREADY,
  SPLAYING,
  SFROZEN,
  SWAITING
};

class StreamStats;

typedef std::vector<StreamStats *> input_stream_info_t;
typedef std::vector<StreamStats *> output_stream_info_t;

struct StreamInfo {
  channel_id_t id;
  StreamType type;
  std::vector<stream_id_t> input;
  std::vector<stream_id_t> output;

  bool Equals(const StreamInfo &inf) const;
};

inline bool operator==(const StreamInfo &left, const StreamInfo &right) {
  return left.Equals(right);
}

inline bool operator!=(const StreamInfo &left, const StreamInfo &right) {
  return !operator==(left, right);
}

struct StreamStruct {
  StreamStruct();
  explicit StreamStruct(const StreamInfo &sha);
  StreamStruct(const StreamInfo &sha, time_t start_time, time_t lst,
               size_t rest);
  StreamStruct(channel_id_t cid, StreamType type, input_stream_info_t input,
               output_stream_info_t output, time_t start_time, time_t lst,
               size_t rest);

  bool IsValid() const;

  ~StreamStruct();

  time_t WithoutRestartTime() const;

  void ResetDataWait();

  const channel_id_t id;
  const StreamType type;

  const time_t start_time; // sec

  time_t loop_start_time;
  size_t restarts;

  const input_stream_info_t input;   // ptrs
  const output_stream_info_t output; // ptrs

  DISALLOW_COPY_AND_ASSIGN(StreamStruct);
};

} // namespace iptv_cloud

namespace common {
std::string ConvertToString(iptv_cloud::StreamStatus st);
}
