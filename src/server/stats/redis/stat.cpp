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

#include "server/stats/redis/stat.h"

namespace iptv_cloud {
namespace server {
namespace stats {
namespace redis {

StatCredentials::StatCredentials(const redis_configuration_t &conf)
    : StatCredentialsBase(STAT_REDIS), conf_(conf) {}

const StatCredentials::redis_configuration_t &StatCredentials::GetConf() const {
  return conf_;
}

Stat::Stat(StatCredentials *creds) : IStat(creds), rc_() {
  CHECK(creds);
  rc_.SetConfig(creds->GetConf());
}

bool Stat::SetKey(const std::string &key, const std::string &value) {
  if (rc_.Connect()) {
    return rc_.Set(key, value);
  }

  return false;
}

bool Stat::GetKey(const std::string &key, std::string *value) {
  if (rc_.Connect()) {
    return rc_.Get(key, value);
  }

  return false;
}

} // namespace redis
} // namespace stats
} // namespace server
} // namespace iptv_cloud
