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

#include "server/commands_info/start_stream_info.h"

#define CONFIG_KEY_FIELD "config"
#define CMD_KEY_FIELD "command_line"

namespace iptv_cloud {
namespace server {

StartStreamInfo::StartStreamInfo() : base_class(), config_(), cmd_() {}

std::string StartStreamInfo::GetConfig() const {
  return config_;
}

std::string StartStreamInfo::GetCmd() const {
  return cmd_;
}

common::Error StartStreamInfo::DoDeSerialize(json_object* serialized) {
  if (!serialized) {
    return common::make_error_inval();
  }

  json_object* jconfig = NULL;
  json_bool jconfig_exists = json_object_object_get_ex(serialized, CONFIG_KEY_FIELD, &jconfig);
  if (!jconfig_exists) {
    return common::make_error_inval();
  }

  json_object* jcmd = NULL;
  json_bool jcmd_exists = json_object_object_get_ex(serialized, CMD_KEY_FIELD, &jcmd);
  if (!jcmd_exists) {
    return common::make_error_inval();
  }

  StartStreamInfo inf;
  inf.config_ = json_object_get_string(jconfig);
  inf.cmd_ = json_object_get_string(jcmd);
  *this = inf;
  return common::Error();
}

common::Error StartStreamInfo::SerializeFields(json_object* out) const {
  json_object_object_add(out, CONFIG_KEY_FIELD, json_object_new_string(config_.c_str()));
  json_object_object_add(out, CMD_KEY_FIELD, json_object_new_string(cmd_.c_str()));
  return common::Error();
}

}  // namespace server
}  // namespace iptv_cloud
