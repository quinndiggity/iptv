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

#include "stream_stats.h"

#include <common/time.h>

namespace iptv_cloud {

StreamStats::StreamStats() : StreamStats(invalid_stream_id) {}

StreamStats::StreamStats(stream_id_t sid)
    : id_(sid), last_update_time_(0), total_bytes_(0), prev_total_bytes_(0),
      bytes_per_second_(0), desire_bytes_per_second_(), is_broken_(true) {}

bool StreamStats::IsValid() const { return id_ != invalid_stream_id; }

stream_id_t StreamStats::GetID() const { return id_; }

time_t StreamStats::GetLastUpdateTime() const { return last_update_time_; }

void StreamStats::SetLastUpdateTime(time_t t) { last_update_time_ = t; }

size_t StreamStats::GetTotalBytes() const { return total_bytes_; }

size_t StreamStats::GetPrevTotalBytes() const { return prev_total_bytes_; }

void StreamStats::SetPrevTotalBytes(size_t bytes) { prev_total_bytes_ = bytes; }

size_t StreamStats::GetDiffTotalBytes() const {
  return total_bytes_ - prev_total_bytes_;
}

void StreamStats::UpdateBps(size_t sec) {
  if (!sec) {
    return;
  }

  bytes_per_second_ = GetDiffTotalBytes() / sec;
}

size_t StreamStats::GetBps() const { return bytes_per_second_; }

void StreamStats::SetBps(size_t bps) { bytes_per_second_ = bps; }

void StreamStats::UpdateCheckPoint() { prev_total_bytes_ = total_bytes_; }

void StreamStats::SetTotalBytes(size_t bytes) {
  total_bytes_ = bytes;
  last_update_time_ = common::time::current_mstime();
}

void StreamStats::SetDesireBytesPerSecond(
    const common::media::DesireBytesPerSec &bps) {
  desire_bytes_per_second_ = bps;
}

common::media::DesireBytesPerSec StreamStats::GetDesireBytesPerSecond() const {
  return desire_bytes_per_second_;
}

void StreamStats::SetIsBroken(bool broken) { is_broken_ = broken; }

bool StreamStats::IsBroken() const { return is_broken_; }

} // namespace iptv_cloud
