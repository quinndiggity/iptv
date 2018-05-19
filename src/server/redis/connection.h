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

#include "server/redis/config.h"

struct redisContext;

namespace iptv_cloud {
namespace server {
namespace redis {

class RedisConnection {
 public:
  RedisConnection();
  ~RedisConnection();

  redis_configuration_t GetConfig() const;
  void SetConfig(const redis_configuration_t& config);

  bool Set(const std::string& key, const std::string& value) const WARN_UNUSED_RESULT;
  bool Get(const std::string& key, std::string* value) const WARN_UNUSED_RESULT;
  bool Publish(const std::string& channel, const std::string& msg) WARN_UNUSED_RESULT;
  bool Connect() WARN_UNUSED_RESULT;
  void DisConnect();
  bool IsConnected() const;

 private:
  struct redisContext* connection_;
  redis_configuration_t config_;
};

}  // namespace redis
}  // namespace server
}  // namespace iptv_cloud
