#include "gtest/gtest.h"

#include "stream/constants.h"

#include "server/options/options.h"
#include "utils/arg_converter.h"

TEST(Options, logo_path) {
  std::string cfg = "{\"" LOGO_PATH_FIELD "\" : \"file:///home/user/logo.png\"}";
  auto args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_EQ(args.size(), 1);
  std::string path;
  bool found = iptv_cloud::utils::ArgsGetValue(args, LOGO_PATH_FIELD, &path);
  ASSERT_TRUE(found);
  ASSERT_EQ(path, "file:///home/user/logo.png");

  cfg = "{\"" LOGO_PATH_FIELD "\" : \"http://home/user/logo.png\"}";
  args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_EQ(args.size(), 1);
  found = iptv_cloud::utils::ArgsGetValue(args, LOGO_PATH_FIELD, &path);
  ASSERT_TRUE(found);
  ASSERT_EQ(path, "http://home/user/logo.png");

  cfg = "{\"" LOGO_PATH_FIELD "\" : \"home://user/logo.png\"}";
  args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_TRUE(args.empty());

  cfg = "{\"" LOGO_PATH_FIELD "\" : \"/home/user/logo.png\"}";
  args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_TRUE(args.empty());

  cfg = "{\"" LOGO_PATH_FIELD "\" : \"/homefile:///user/logo.png\"}";
  args = iptv_cloud::server::options::ValidateConfig(cfg);
  ASSERT_TRUE(args.empty());
}
