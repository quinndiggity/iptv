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

#include "server/commands_info/restart_stream_info.h"

#define STREAM_ID_FIELD "id"

namespace iptv_cloud {
namespace server {

RestartStreamInfo::RestartStreamInfo() : base_class(), stream_id_() {}

RestartStreamInfo::RestartStreamInfo(const std::string& stream_id) : stream_id_(stream_id) {}

std::string RestartStreamInfo::GetStreamID() const {
  return stream_id_;
}

common::Error RestartStreamInfo::DoDeSerialize(json_object* serialized) {
  RestartStreamInfo inf;
  json_object* jid = NULL;
  json_bool jid_exists = json_object_object_get_ex(serialized, STREAM_ID_FIELD, &jid);
  if (!jid_exists) {
    return common::make_error_inval();
  }

  inf.stream_id_ = json_object_get_string(jid);
  *this = inf;
  return common::Error();
}

common::Error RestartStreamInfo::SerializeFields(json_object* out) const {
  json_object_object_add(out, STREAM_ID_FIELD, json_object_new_string(stream_id_.c_str()));
  return common::Error();
}

}  // namespace server
}  // namespace iptv_cloud
