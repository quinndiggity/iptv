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

#include "stream_commands_info/stream_stats_info.h"

#define FIELD_STREAM_STATS_ID "id"
#define FIELD_STREAM_STATS_LAST_UPDATE_TIME "last_update_time"
#define FIELD_STREAM_STATS_PREV_TOTAL_BYTES "prev_total_bytes"
#define FIELD_STREAM_STATS_TOTAL_BYTES "total_bytes"
#define FIELD_STREAM_STATS_BYTES_PER_SECOND "bps"
#define FIELD_STREAM_STATS_DESIRE_BYTES_PER_SECOND "dbps"
#define FIELD_STREAM_STATS_IS_BROKEN "is_broken"

namespace iptv_cloud {

StreamStatsInfo::StreamStatsInfo() : StreamStatsInfo(StreamStats()) {}

StreamStatsInfo::StreamStatsInfo(const StreamStats& stats) : stats_(stats) {}

StreamStats StreamStatsInfo::GetStats() const {
  return stats_;
}

common::Error StreamStatsInfo::SerializeFields(json_object* out) const {
  if (!stats_.IsValid()) {
    return common::make_error_inval();
  }

  stream_id_t sid = stats_.GetID();
  json_object_object_add(out, FIELD_STREAM_STATS_ID, json_object_new_int64(sid));

  time_t last = stats_.GetLastUpdateTime();
  json_object_object_add(out, FIELD_STREAM_STATS_LAST_UPDATE_TIME, json_object_new_int64(last));

  size_t prev_t = stats_.GetPrevTotalBytes();
  json_object_object_add(out, FIELD_STREAM_STATS_PREV_TOTAL_BYTES, json_object_new_int64(prev_t));

  size_t tot = stats_.GetPrevTotalBytes();
  json_object_object_add(out, FIELD_STREAM_STATS_TOTAL_BYTES, json_object_new_int64(tot));

  size_t bps = stats_.GetBps();
  json_object_object_add(out, FIELD_STREAM_STATS_BYTES_PER_SECOND, json_object_new_int64(bps));

  common::media::DesireBytesPerSec dbps = stats_.GetDesireBytesPerSecond();
  std::string dbps_str = common::ConvertToString(dbps);
  json_object_object_add(out, FIELD_STREAM_STATS_DESIRE_BYTES_PER_SECOND, json_object_new_string(dbps_str.c_str()));

  bool is_broken = stats_.IsBroken();
  json_object_object_add(out, FIELD_STREAM_STATS_IS_BROKEN, json_object_new_boolean(is_broken));

  return common::Error();
}

common::Error StreamStatsInfo::DoDeSerialize(json_object* serialized) {
  json_object* jid = NULL;
  json_bool jid_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_ID, &jid);
  if (!jid_exists) {
    return common::make_error_inval();
  }
  StreamStats stats(json_object_get_int64(jid));

  json_object* jlut = NULL;
  json_bool jlut_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_LAST_UPDATE_TIME, &jlut);
  if (jlut_exists) {
    stats.SetLastUpdateTime(json_object_get_int64(jlut));
  }

  json_object* jptb = NULL;
  json_bool jptb_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_PREV_TOTAL_BYTES, &jptb);
  if (jptb_exists) {
    stats.SetPrevTotalBytes(json_object_get_int64(jptb));
  }

  json_object* jtb = NULL;
  json_bool jtb_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_TOTAL_BYTES, &jtb);
  if (jtb_exists) {
    stats.SetTotalBytes(json_object_get_int64(jtb));
  }

  json_object* jbps = NULL;
  json_bool jbps_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_BYTES_PER_SECOND, &jbps);
  if (jbps_exists) {
    stats.SetBps(json_object_get_int64(jbps));
  }

  json_object* jdbps = NULL;
  json_bool jdbps_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_DESIRE_BYTES_PER_SECOND, &jdbps);
  common::media::DesireBytesPerSec dbps;
  if (jdbps_exists && common::ConvertFromString(json_object_get_string(jdbps), &dbps)) {
    stats.SetDesireBytesPerSecond(dbps);
  }

  json_object* jbroken = NULL;
  json_bool jbroken_exists = json_object_object_get_ex(serialized, FIELD_STREAM_STATS_IS_BROKEN, &jbroken);
  if (jbroken_exists) {
    stats.SetIsBroken(jbroken);
  }
  *this = StreamStatsInfo(stats);
  return common::Error();
}

}  // namespace iptv_cloud
