#include "gtest/gtest.h"

#include <common/macros.h>

#include "output_uri.h"
#include "stream/constants.h"

#define RTMP_OUTPUT "rtmp://4.31.30.153:1935/devapp/tokengenffmpeg1"
#define HTTP_OUTPUT "/home/sasha/123/"

TEST(OutputUri, ConvertFromString) {
  const std::string invalid_uri_json =
      "{ \"" FIELD_OUTPUT_ID "\": 0, \"" FIELD_OUTPUT_URI "\": \"\", \"" FIELD_OUTPUT_HTTP_ROOT
      "\": \"\", \"" FIELD_OUTPUT_WIDTH
      "\": " STRINGIZE(DEFAULT_VIDEO_WIDTH) ", \"" FIELD_OUTPUT_HEIGHT "\": " STRINGIZE(
          DEFAULT_VIDEO_HEIGHT) ", \"" FIELD_OUTPUT_VIDEO_BITRATE
                                "\": " STRINGIZE(DEFAULT_VIDEO_BITRATE) ", \"" FIELD_OUTPUT_AUDIO_BITRATE
                                                                        "\": " STRINGIZE(DEFAULT_AUDIO_BITRATE) " }";
  iptv_cloud::OutputUri invalid_uri;
  ASSERT_EQ(invalid_uri.GetID(), 0);
  ASSERT_EQ(invalid_uri.GetOutput(), common::uri::Url());
  ASSERT_EQ(invalid_uri.GetHttpRoot(), common::file_system::ascii_file_string_path());
  std::string conv = common::ConvertToString(invalid_uri);
  ASSERT_EQ(conv, invalid_uri_json);

  const std::string uri_json =
      "{ \"" FIELD_OUTPUT_ID "\": 1, \"" FIELD_OUTPUT_URI "\": \"" RTMP_OUTPUT "\", \"" FIELD_OUTPUT_HTTP_ROOT
      "\": \"" HTTP_OUTPUT "\", \"" FIELD_OUTPUT_WIDTH
      "\": " STRINGIZE(DEFAULT_VIDEO_WIDTH) ", \"" FIELD_OUTPUT_HEIGHT "\": " STRINGIZE(
          DEFAULT_VIDEO_HEIGHT) ", \"" FIELD_OUTPUT_VIDEO_BITRATE
                                "\": " STRINGIZE(DEFAULT_VIDEO_BITRATE) ", \"" FIELD_OUTPUT_AUDIO_BITRATE
                                                                        "\": " STRINGIZE(DEFAULT_AUDIO_BITRATE) " }";
  iptv_cloud::OutputUri uri;
  ASSERT_TRUE(common::ConvertFromString(uri_json, &uri));
  ASSERT_EQ(uri.GetID(), 1);
  common::uri::Url ro(RTMP_OUTPUT);
  ASSERT_EQ(uri.GetOutput(), ro);
  ASSERT_EQ(uri.GetHttpRoot(), common::file_system::ascii_directory_string_path(HTTP_OUTPUT));
  conv = common::ConvertToString(uri);
  ASSERT_EQ(conv, uri_json);
}
