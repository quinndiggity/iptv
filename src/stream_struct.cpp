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

#include "stream_struct.h"

#include <common/time.h>

#include "stream_stats.h"

namespace common {

std::string ConvertToString(iptv_cloud::StreamStatus st) {
  static const std::string StreamStatuses[] = {
      "New", "Inited", "Started", "Ready", "Streaming", "Frozen", "Waiting",
  };

  return StreamStatuses[st];
}

} // namespace common

namespace iptv_cloud {

namespace {
output_stream_info_t make_outputs(const std::vector<stream_id_t> &output) {
  output_stream_info_t res;
  for (stream_id_t out : output) {
    res.push_back(new StreamStats(out));
  }
  return res;
}

input_stream_info_t make_inputs(const std::vector<stream_id_t> &input) {
  input_stream_info_t res;
  for (stream_id_t in : input) {
    res.push_back(new StreamStats(in));
  }
  return res;
}
} // namespace

bool StreamInfo::Equals(const StreamInfo &inf) const {
  return id == inf.id && type == inf.type && input == inf.input &&
         output == inf.output;
}

StreamStruct::StreamStruct() : StreamStruct(StreamInfo()) {}

StreamStruct::StreamStruct(const StreamInfo &sha)
    : StreamStruct(sha, common::time::current_mstime() / 1000, 0, 0) {}

StreamStruct::StreamStruct(const StreamInfo &sha, time_t start_time, time_t lst,
                           size_t rest)
    : StreamStruct(sha.id, sha.type, make_inputs(sha.input),
                   make_outputs(sha.output), start_time, lst, rest) {}

StreamStruct::StreamStruct(channel_id_t cid, StreamType type,
                           input_stream_info_t input,
                           output_stream_info_t output, time_t start_time,
                           time_t lst, size_t rest)
    : id(cid), type(type), start_time(start_time), loop_start_time(lst),
      restarts(rest), input(input), output(output) {}

bool StreamStruct::IsValid() const { return !id.empty(); }

StreamStruct::~StreamStruct() {
  for (size_t i = 0; i < output.size(); ++i) {
    delete output[i];
  }

  for (size_t i = 0; i < input.size(); ++i) {
    delete input[i];
  }
}

time_t StreamStruct::WithoutRestartTime() const {
  const time_t current_time = common::time::current_mstime() / 1000;
  if (!loop_start_time) {
    return 0;
  }

  return current_time - loop_start_time;
}

void StreamStruct::ResetDataWait() {
  for (size_t i = 0; i < input.size(); ++i) {
    input[i]->UpdateCheckPoint();
  }
  for (size_t i = 0; i < output.size(); ++i) {
    output[i]->UpdateCheckPoint();
  }
}

} // namespace iptv_cloud
