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

#include "server/stats/stat_credentials.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "fake/stat.h"
#ifdef HAVE_REDIS
#include "server/stats/redis/redis_info.h"
#include "server/stats/redis/stat.h"
#endif

#define FIELD_TYPE "type"
#define FIELD_CREDS "creds"

namespace iptv_cloud {
namespace server {
namespace stats {

StatCredentialsBase *StatCredentialsBase::CreateCreadentialsFromString(
    const std::string &json_data) {
  StatCredentialsBase *result = nullptr;
  if (!IsValidCredentials(json_data, &result)) {
    return nullptr;
  }

  return result;
}

bool StatCredentialsBase::IsValidCredentials(const std::string &json_data,
                                             StatCredentialsBase **creds) {
  if (json_data.empty()) {
    return false;
  }

  json_object *obj = json_tokener_parse(json_data.c_str());
  if (!obj) {
    return false;
  }

  json_object *jtype = NULL;
  json_bool jtype_exists = json_object_object_get_ex(obj, FIELD_TYPE, &jtype);
  if (!jtype_exists) {
    json_object_put(obj);
    return false;
  }

  json_object *jcreds = NULL;
  json_bool jcreds_exists =
      json_object_object_get_ex(obj, FIELD_CREDS, &jcreds);
  if (!jcreds_exists) {
    json_object_put(obj);
    return false;
  }

  StatType bt = static_cast<StatType>(json_object_get_int(jtype));
  if (bt == STAT_FAKE) {
    if (creds) {
      *creds = new fake::StatCredentials;
    }
    json_object_put(obj);
    return true;
  }
#ifdef HAVE_REDIS
  else if (bt == STAT_REDIS) {
    redis::RedisInfo rinf;
    common::Error err = rinf.DeSerialize(jcreds);
    if (err) {
      json_object_put(obj);
      return false;
    }
    if (creds) {
      *creds = new redis::StatCredentials(rinf.GetConfig());
    }
    json_object_put(obj);
    return true;
  }
#endif
  json_object_put(obj);
  return false;
}

StatType StatCredentialsBase::GetType() const { return type_; }

StatCredentialsBase::StatCredentialsBase(StatType type) : type_(type) {}

} // namespace stats
} // namespace server
} // namespace iptv_cloud
