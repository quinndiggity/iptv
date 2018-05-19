#ifdef HAVE_REDIS
#include <common/convert2string.h>

#include "server/redis/connection.h"

void TestSyncRedisConnection() {
  iptv_cloud::server::redis::redis_configuration_t config;
  config.redis_host = common::net::HostAndPort::CreateLocalHost(6379);

  iptv_cloud::server::redis::RedisConnection redis;
  redis.SetConfig(config);
  CHECK(redis.Connect());
  int random = std::rand();
  CHECK(redis.Set("test_key", common::ConvertToString(random)));
  std::string value;
  CHECK(redis.Get("test_key", &value));
  int i;
  CHECK(common::ConvertFromString(value, &i));
  CHECK_EQ(random, i);
}

#endif

int main(int argc, char** argv) {
  UNUSED(argc);
  UNUSED(argv);

#ifdef HAVE_REDIS
  TestSyncRedisConnection();
#endif
  return EXIT_SUCCESS;
}
