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

#include <common/media/bandwidth_estimation.h>

#include "types.h"

namespace iptv_cloud {

class StreamStats { // only compile time size fields
public:
  StreamStats();
  explicit StreamStats(stream_id_t sid);

  bool IsValid() const;

  stream_id_t GetID() const;

  time_t GetLastUpdateTime() const;
  void SetLastUpdateTime(time_t t);

  size_t GetTotalBytes() const;
  void SetTotalBytes(size_t bytes);

  size_t GetPrevTotalBytes() const;
  void SetPrevTotalBytes(size_t bytes);

  size_t GetDiffTotalBytes() const;

  void UpdateBps(size_t sec);
  size_t GetBps() const;
  void SetBps(size_t bps);

  void UpdateCheckPoint();

  void SetDesireBytesPerSecond(const common::media::DesireBytesPerSec &bps);
  common::media::DesireBytesPerSec GetDesireBytesPerSecond() const;

  void SetIsBroken(bool broken);
  bool IsBroken() const;

private:
  stream_id_t id_;

  time_t last_update_time_; // up_time
  size_t total_bytes_;      // received bytes
  size_t prev_total_bytes_; // checkpoint received bytes
  size_t bytes_per_second_; // bps

  common::media::DesireBytesPerSec desire_bytes_per_second_;

  bool is_broken_;
};

} // namespace iptv_cloud
