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

#include "stream_commands_info/stream_struct_info.h"

#include <math.h>

#include "stream_stats_info.h"

#define FIELD_JOB_ID "id"
#define FIELD_JOB_TYPE "type"
//#define FIELD_JOB_UPTIME "uptime"
#define FIELD_JOB_CPU "cpu"
#define FIELD_JOB_RSS "rss"
#define FIELD_JOB_STATUS "status"
#define FIELD_JOB_LOOP_START_TIME "loop_start_time"
#define FIELD_JOB_RESTARTS "restarts"
#define FIELD_JOB_START_TIME "start_time"
#define FIELD_JOB_CUR_TIME "cur_time"

#define FIELD_JOB_INPUT_STREAMS "input_streams"
/*#define FIELD_JOB_INPUT_STREAM_ID "id"
#define FIELD_JOB_INPUT_STREAM_LAST_PULL_TIME "last_pull_time"
#define FIELD_JOB_INPUT_STREAM_BYTES_IN "bytes_in"
#define FIELD_JOB_INPUT_STREAM_BYTES_PER_SEC_IN "in_bps"
#define FIELD_JOB_INPUT_STREAM_IS_BROKEN "is_broken"*/

#define FIELD_JOB_OUTPUT_STREAMS "output_streams"
/*#define FIELD_JOB_OUTPUT_STREAM_ID "id"
#define FIELD_JOB_OUTPUT_STREAM_LAST_PUSH_TIME "last_push_time"
#define FIELD_JOB_OUTPUT_STREAM_BYTES_OUT "bytes_out"
#define FIELD_JOB_OUTPUT_STREAM_BYTES_PER_SEC_OUT "out_bps"
#define FIELD_JOB_OUTPUT_STREAM_IS_BROKEN "is_broken"*/

namespace iptv_cloud {

StreamStructInfo::StreamStructInfo()
    : stream_struct_(), status_(), cpu_load_(), rss_(), time_() {}

StreamStructInfo::StreamStructInfo(const StreamStruct &str, StreamStatus st,
                                   cpu_load_t cpu_load, rss_t rss, time_t time)
    : stream_struct_(), status_(st), cpu_load_(cpu_load), rss_(rss),
      time_(time) {
  input_stream_info_t input;
  for (auto it = str.input.rbegin(); it != str.input.rend(); ++it) {
    StreamStats copy = *(*it);
    input.push_back(new StreamStats(copy));
  }
  output_stream_info_t output;
  for (auto it = str.output.rbegin(); it != str.output.rend(); ++it) {
    StreamStats copy = *(*it);
    output.push_back(new StreamStats(copy));
  }
  StreamStruct *struc =
      new StreamStruct(str.id, str.type, input, output, str.start_time,
                       str.loop_start_time, str.restarts);
  stream_struct_.reset(struc);

  /*cpu_load_t cpu_load = cpu_load_;
  if (isnan(cpu_load_) || isinf(cpu_load_)) {
    cpu_load = 0.0;
  }*/
}

StreamStructInfo::stream_struct_t StreamStructInfo::GetStreamStruct() const {
  return stream_struct_;
}

StreamStatus StreamStructInfo::GetStatus() const { return status_; }

StreamStructInfo::cpu_load_t StreamStructInfo::GetCpuLoad() const {
  return cpu_load_;
}

StreamStructInfo::rss_t StreamStructInfo::GetRss() const { return rss_; }

time_t StreamStructInfo::GetTime() const { return time_; }

common::Error StreamStructInfo::SerializeFields(json_object *out) const {
  if (!stream_struct_ || !stream_struct_->IsValid()) {
    return common::make_error_inval();
  }

  const channel_id_t channel_id = stream_struct_->id;
  json_object_object_add(out, FIELD_JOB_ID,
                         json_object_new_string(channel_id.c_str()));
  json_object_object_add(out, FIELD_JOB_TYPE,
                         json_object_new_int(stream_struct_->type));

  input_stream_info_t input_streams = stream_struct_->input;
  json_object *jinput_streams = json_object_new_array();
  for (StreamStats *inf : input_streams) {
    json_object *jinf = NULL;
    StreamStatsInfo sinf(*inf);
    common::Error err = sinf.Serialize(&jinf);
    if (err) {
      continue;
    }
    json_object_array_add(jinput_streams, jinf);
  }
  json_object_object_add(out, FIELD_JOB_INPUT_STREAMS, jinput_streams);

  output_stream_info_t output_streams = stream_struct_->output;
  json_object *joutput_streams = json_object_new_array();
  for (StreamStats *inf : output_streams) {
    json_object *jinf = NULL;
    StreamStatsInfo sinf(*inf);
    common::Error err = sinf.Serialize(&jinf);
    if (err) {
      continue;
    }
    json_object_array_add(joutput_streams, jinf);
  }
  json_object_object_add(out, FIELD_JOB_OUTPUT_STREAMS, joutput_streams);

  json_object_object_add(
      out, FIELD_JOB_LOOP_START_TIME,
      json_object_new_int64(stream_struct_->loop_start_time));
  json_object_object_add(out, FIELD_JOB_RSS, json_object_new_int64(rss_));
  json_object_object_add(out, FIELD_JOB_CPU, json_object_new_double(cpu_load_));
  json_object_object_add(out, FIELD_JOB_STATUS, json_object_new_int(status_));
  json_object_object_add(out, FIELD_JOB_RESTARTS,
                         json_object_new_int64(stream_struct_->restarts));
  json_object_object_add(out, FIELD_JOB_START_TIME,
                         json_object_new_int(stream_struct_->start_time));
  json_object_object_add(out, FIELD_JOB_CUR_TIME, json_object_new_int64(time_));
  return common::Error();
}

common::Error StreamStructInfo::DoDeSerialize(json_object *serialized) {
  json_object *jid = NULL;
  json_bool jid_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_ID, &jid);
  if (!jid_exists) {
    return common::make_error_inval();
  }
  channel_id_t cid = json_object_get_string(jid);

  json_object *jtype = NULL;
  json_bool jtype_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_TYPE, &jtype);
  if (!jtype_exists) {
    return common::make_error_inval();
  }
  StreamType type = static_cast<StreamType>(json_object_get_int(jtype));

  input_stream_info_t input;
  json_object *jinput = NULL;
  json_bool jinput_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_INPUT_STREAMS, &jinput);
  if (jinput_exists) {
    int len = json_object_array_length(jinput);
    for (int i = len - 1; i >= 0; --i) {
      json_object *jin = json_object_array_get_idx(jinput, i);
      StreamStatsInfo sinf;
      common::Error err = sinf.DeSerialize(jin);
      if (err) {
        continue;
      }

      input.push_back(new StreamStats(sinf.GetStats()));
    }
  }

  output_stream_info_t output;
  json_object *joutput = NULL;
  json_bool joutput_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_OUTPUT_STREAMS, &joutput);
  if (joutput_exists) {
    int len = json_object_array_length(joutput);
    for (int i = len - 1; i >= 0; --i) {
      json_object *jin = json_object_array_get_idx(joutput, i);
      StreamStatsInfo sinf;
      common::Error err = sinf.DeSerialize(jin);
      if (err) {
        continue;
      }

      output.push_back(new StreamStats(sinf.GetStats()));
    }
  }

  StreamStatus st = SNEW;
  json_object *jstatus = NULL;
  json_bool jstatus_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_STATUS, &jstatus);
  if (jstatus_exists) {
    st = static_cast<StreamStatus>(json_object_get_int(jstatus));
  }

  cpu_load_t cpu_load = 0;
  json_object *jcpu_load = NULL;
  json_bool jcpu_load_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_CPU, &jcpu_load);
  if (jcpu_load_exists) {
    cpu_load = json_object_get_double(jcpu_load);
  }

  rss_t rss = 0;
  json_object *jrss = NULL;
  json_bool jrss_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_RSS, &jrss);
  if (jrss_exists) {
    rss = json_object_get_int(jrss);
  }

  time_t time = 0;
  json_object *jtime = NULL;
  json_bool jtime_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_CUR_TIME, &jtime);
  if (jtime_exists) {
    time = json_object_get_int(jtime);
  }

  time_t start_time = 0;
  json_object *jstart_time = NULL;
  json_bool jstart_time_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_START_TIME, &jstart_time);
  if (jstart_time_exists) {
    start_time = json_object_get_int(jstart_time);
  }

  size_t restarts = 0;
  json_object *jrestarts = NULL;
  json_bool jrestarts_exists =
      json_object_object_get_ex(serialized, FIELD_JOB_RESTARTS, &jrestarts);
  if (jrestarts_exists) {
    restarts = json_object_get_int64(jrestarts);
  }

  time_t loop_start_time = 0;
  json_object *jloop_start_time = NULL;
  json_bool jloop_start_time_exists = json_object_object_get_ex(
      serialized, FIELD_JOB_LOOP_START_TIME, &jloop_start_time);
  if (jloop_start_time_exists) {
    loop_start_time = json_object_get_int64(jloop_start_time);
  }

  StreamStruct strct(cid, type, input, output, start_time, loop_start_time,
                     restarts);
  *this = StreamStructInfo(strct, st, cpu_load, rss, time);
  return common::Error();
}

} // namespace iptv_cloud
