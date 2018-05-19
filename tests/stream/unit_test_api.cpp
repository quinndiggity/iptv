#include "gtest/gtest.h"

#include "stream/streams/configs/encoding_config.h"
#include "stream/streams/configs/relay_config.h"

#include "stream/configs_factory.h"
#include "stream/constants.h"

TEST(Api, init) {
  iptv_cloud::utils::ArgsMap emp;
  iptv_cloud::stream::Config* empty_api = iptv_cloud::stream::make_config(emp);
  ASSERT_FALSE(empty_api);
  iptv_cloud::output_t ouri;
  iptv_cloud::OutputUri uri;
  uri.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri);

  const std::string uri_str = common::ConvertToString(ouri);
  iptv_cloud::input_t iurls;
  iurls.push_back((iptv_cloud::InputUri(0, common::uri::Url(TEST_URL))));
  const iptv_cloud::utils::ArgsMap required_args = {
      std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
      std::make_pair(FEEDBACK_DIR_FIELD, "~"), std::make_pair(OUTPUT_FIELD, uri_str)};
  const iptv_cloud::utils::ArgsMap required_args2 = {
      std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
      std::make_pair(FEEDBACK_DIR_FIELD, "~~"), std::make_pair(OUTPUT_FIELD, uri_str)};
  iptv_cloud::stream::Config* api = iptv_cloud::stream::make_config(required_args);
  ASSERT_NE(api, empty_api);

  iptv_cloud::stream::Config* api2 = iptv_cloud::stream::make_config(required_args2);
  ASSERT_EQ(*api, *api2);  // FEEDBACK_DIR_FIELD didn't save

  iptv_cloud::utils::ArgsMap args_screen = {std::make_pair(TYPE_FIELD, "screen"),
                                            std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
                                            std::make_pair(FEEDBACK_DIR_FIELD, "~"),
                                            std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(ID_FIELD, "screen")};
  iptv_cloud::stream::Config* api1 = iptv_cloud::stream::make_config(args_screen);
  ASSERT_TRUE(api1);
  args_screen.push_back(std::make_pair("volume", "10.0"));
  iptv_cloud::stream::Config* api22 = iptv_cloud::stream::make_config(args_screen);
  ASSERT_TRUE(api22);
  ASSERT_EQ(*api1, *api22);  // volume not exist in screen

  ASSERT_TRUE(iptv_cloud::IsTestUrl(iptv_cloud::InputUri(0, common::uri::Url(TEST_URL))));

  ASSERT_TRUE(iptv_cloud::stream::IsScreenUrl(common::uri::Url(SCREEN_URL)));
  ASSERT_TRUE(iptv_cloud::stream::IsRecordingUrl(common::uri::Url(RECORDING_URL)));
}

TEST(Api, logo) {
  iptv_cloud::output_t ouri;
  iptv_cloud::OutputUri uri2;
  uri2.SetOutput(common::uri::Url("screen"));
  ouri.push_back(uri2);

  const std::string uri_str = common::ConvertToString(ouri);

  iptv_cloud::input_t iurls;
  iurls.push_back((iptv_cloud::InputUri(0, common::uri::Url(TEST_URL))));
  iptv_cloud::utils::ArgsMap args = {
      std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
      std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(LOGO_PATH_FIELD, "file:///home/user/logo.png")};
  iptv_cloud::stream::Config* api = iptv_cloud::stream::make_config(args);
  ASSERT_TRUE(api);
  common::uri::Url uri = static_cast<iptv_cloud::stream::streams::EncodingConfig*>(api)->GetLogoPath();

  ASSERT_TRUE(uri.IsValid());
  ASSERT_EQ(uri.GetScheme(), common::uri::Url::file);
  common::uri::Upath upath = uri.GetPath();
  ASSERT_EQ(upath.GetPath(), "/home/user/logo.png");

  args = {std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
          std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(LOGO_PATH_FIELD, "/home/user/logo.png")};
  api = iptv_cloud::stream::make_config(args);
  ASSERT_TRUE(api);
  uri = static_cast<iptv_cloud::stream::streams::EncodingConfig*>(api)->GetLogoPath();
  ASSERT_FALSE(uri.IsValid());

  args = {std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
          std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(LOGO_PATH_FIELD, "http://home/user/logo.png")};
  api = iptv_cloud::stream::make_config(args);
  ASSERT_TRUE(api);
  uri = static_cast<iptv_cloud::stream::streams::EncodingConfig*>(api)->GetLogoPath();
  ASSERT_TRUE(uri.IsValid());
  ASSERT_EQ(uri.GetScheme(), common::uri::Url::http);
  upath = uri.GetPath();
  ASSERT_EQ(upath.GetPath(), "user/logo.png");

  args = {std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
          std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(FEEDBACK_DIR_FIELD, "~")};
  api = iptv_cloud::stream::make_config(args);
  ASSERT_TRUE(api);
  uri = static_cast<iptv_cloud::stream::streams::EncodingConfig*>(api)->GetLogoPath();
  ASSERT_FALSE(uri.IsValid());

  args = {std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
          std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(LOGO_PATH_FIELD, "/homefile://user/logo.png")};
  api = iptv_cloud::stream::make_config(args);
  ASSERT_TRUE(api);
  uri = static_cast<iptv_cloud::stream::streams::EncodingConfig*>(api)->GetLogoPath();
  ASSERT_FALSE(uri.IsValid());

  args = {std::make_pair(TYPE_FIELD, "encoding"), std::make_pair(INPUT_FIELD, common::ConvertToString(iurls)),
          std::make_pair(OUTPUT_FIELD, uri_str), std::make_pair(LOGO_PATH_FIELD, "home://user/logo.png")};
  api = iptv_cloud::stream::make_config(args);
  ASSERT_TRUE(api);
  uri = static_cast<iptv_cloud::stream::streams::EncodingConfig*>(api)->GetLogoPath();
  ASSERT_FALSE(uri.IsValid());
}
