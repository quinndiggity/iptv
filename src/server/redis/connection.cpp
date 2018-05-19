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

#include "server/redis/connection.h"

#include <hiredis/hiredis.h>

namespace iptv_cloud {
namespace server {
namespace redis {

namespace {

struct redisContext* redis_connect(const redis_configuration_t& config) {
  const common::net::HostAndPort redis_host = config.redis_host;
  const std::string unix_path = config.redis_unix_socket;

  if (!redis_host.IsValid() && unix_path.empty()) {
    return NULL;
  }

  struct redisContext* redis = NULL;
  std::string host_str = redis_host.GetHost();
  if (unix_path.empty()) {
    redis = redisConnect(host_str.c_str(), redis_host.GetPort());
  } else {
    redis = redisConnectUnix(unix_path.c_str());
    if (!redis || redis->err) {
      if (redis) {
        ERROR_LOG() << "Redis UNIX connection error: " << redis->errstr << ", path: " << unix_path;
        redisFree(redis);
        redis = NULL;
      }
      redis = redisConnect(host_str.c_str(), redis_host.GetPort());
    }
  }

  if (!redis || redis->err) {
    if (redis) {
      ERROR_LOG() << "Redis connection error: " << redis->errstr;
      redisFree(redis);
      return NULL;
    }

    ERROR_LOG() << "Failed connect to redis host: " << common::ConvertToString(redis_host)
                << " unix path: " << unix_path;
    return NULL;
  }

  return redis;
}

}  // namespace

RedisConnection::RedisConnection() : connection_(NULL) {}

redis_configuration_t RedisConnection::GetConfig() const {
  return config_;
}

void RedisConnection::SetConfig(const redis_configuration_t& config) {
  config_ = config;
}

RedisConnection::~RedisConnection() {
  DisConnect();
}

bool RedisConnection::Set(const std::string& key, const std::string& value) const {
  if (key.empty() || value.empty()) {
    return false;
  }

  if (!IsConnected()) {
    return false;
  }

  void* reply = redisCommand(connection_, "SET %s %s", key.c_str(), value.c_str());
  if (!reply) {
    ERROR_LOG() << "REDIS SET CONNECTION ERROR: " << connection_->errstr;
    return false;
  }

  freeReplyObject(reply);
  return true;
}

bool RedisConnection::Get(const std::string& key, std::string* value) const {
  if (key.empty() || !value) {
    return false;
  }

  if (!IsConnected()) {
    return false;
  }

  void* reply = redisCommand(connection_, "GET %s", key.c_str());
  if (!reply) {
    ERROR_LOG() << "REDIS GET CONNECTION ERROR: " << connection_->errstr;
    return false;
  }

  redisReply* rreply = static_cast<redisReply*>(reply);
  *value = std::string(rreply->str, rreply->len);
  freeReplyObject(reply);
  return true;
}

bool RedisConnection::Publish(const std::string& channel, const std::string& msg) {
  if (channel.empty() || msg.empty()) {
    return false;
  }

  if (!IsConnected()) {
    return false;
  }

  void* rreply = redisCommand(connection_, "PUBLISH %s %s", channel.c_str(), msg.c_str());
  if (!rreply) {
    ERROR_LOG() << "REDIS PUB CONNECTION ERROR: " << connection_->errstr;
    return false;
  }

  freeReplyObject(rreply);
  return true;
}

bool RedisConnection::Connect() {
  if (IsConnected()) {
    return true;
  }

  DisConnect();
  connection_ = redis_connect(config_);
  return connection_ != NULL;
}

void RedisConnection::DisConnect() {
  if (!connection_) {
    return;
  }

  redisFree(connection_);
  connection_ = NULL;
}

bool RedisConnection::IsConnected() const {
  return connection_ != NULL && !connection_->err;
}

}  // namespace redis
}  // namespace server
}  // namespace iptv_cloud
