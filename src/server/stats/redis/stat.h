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

#include "server/stats/istat.h"  // for IStat

#include "server/redis/connection.h"  // for RedisConnection, redis_configuration_t...

namespace iptv_cloud {
namespace server {
namespace stats {
namespace redis {

class StatCredentials : public StatCredentialsBase {
 public:
  typedef iptv_cloud::server::redis::redis_configuration_t redis_configuration_t;
  StatCredentials(const redis_configuration_t& conf);
  const redis_configuration_t& GetConf() const;

 private:
  const redis_configuration_t conf_;
};

class Stat : public IStat {
 public:
  typedef iptv_cloud::server::redis::RedisConnection RedisConnection;
  Stat(StatCredentials* creds);  // take ownership

  virtual bool SetKey(const std::string& key, const std::string& value) override;
  virtual bool GetKey(const std::string& key, std::string* value) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(Stat);
  RedisConnection rc_;
};

}  // namespace redis
}  // namespace stats
}  // namespace server
}  // namespace iptv_cloud
