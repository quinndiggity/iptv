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

#include <common/serializer/json_serializer.h>

#include "stream_stats.h"

namespace iptv_cloud {

class StreamStatsInfo : public common::serializer::JsonSerializer<StreamStatsInfo> {
 public:
  typedef JsonSerializer<StreamStatsInfo> base_class;
  StreamStatsInfo();
  explicit StreamStatsInfo(const StreamStats& stats);

  StreamStats GetStats() const;

 protected:
  virtual common::Error DoDeSerialize(json_object* serialized) override;
  virtual common::Error SerializeFields(json_object* out) const override;

 private:
  StreamStats stats_;
};

}  // namespace iptv_cloud
