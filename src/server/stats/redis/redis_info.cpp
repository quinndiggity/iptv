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

#include "server/stats/redis/redis_info.h"

#define FIELD_REDIS_HOST_FIELD "host"
#define FIELD_REDIS_UNIX_SOCKET_FIELD "unix_socket"

namespace iptv_cloud {
namespace server {
namespace stats {
namespace redis {

RedisInfo::RedisInfo() : base_class(), config_() {}

RedisInfo::redis_configuration_t RedisInfo::GetConfig() const {
  return config_;
}

common::Error RedisInfo::SerializeFields(json_object *out) const {
  std::string host_str = common::ConvertToString(config_.redis_host);
  json_object_object_add(out, FIELD_REDIS_HOST_FIELD,
                         json_object_new_string(host_str.c_str()));
  json_object_object_add(
      out, FIELD_REDIS_UNIX_SOCKET_FIELD,
      json_object_new_string(config_.redis_unix_socket.c_str()));
  return common::Error();
}

common::Error RedisInfo::DoDeSerialize(json_object *serialized) {
  UNUSED(serialized);
  RedisInfo inf;
  json_object *jhost = NULL;
  json_bool jhost_exists =
      json_object_object_get_ex(serialized, FIELD_REDIS_HOST_FIELD, &jhost);
  if (jhost_exists) {
    std::string host_str = json_object_get_string(jhost);
    common::net::HostAndPort lhs;
    if (common::ConvertFromString(host_str, &lhs)) {
      inf.config_.redis_host = lhs;
    }
  }

  json_object *junix = NULL;
  json_bool junix_exists = json_object_object_get_ex(
      serialized, FIELD_REDIS_UNIX_SOCKET_FIELD, &junix);
  if (junix_exists) {
    inf.config_.redis_unix_socket = json_object_get_string(junix);
  }
  *this = inf;
  return common::Error();
}

} // namespace redis
} // namespace stats
} // namespace server
} // namespace iptv_cloud
