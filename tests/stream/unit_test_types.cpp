#include <gtest/gtest.h>

#include "stream/stypes.h"

TEST(element_id_t, GetElementId) {
  iptv_cloud::stream::element_id_t id;
  ASSERT_FALSE(iptv_cloud::stream::GetElementId("udv_", nullptr));
  ASSERT_FALSE(iptv_cloud::stream::GetElementId("udv_", &id));
  ASSERT_FALSE(iptv_cloud::stream::GetElementId("udv_42_udv", &id));
  ASSERT_FALSE(iptv_cloud::stream::GetElementId("udv42", &id));
  ASSERT_FALSE(iptv_cloud::stream::GetElementId("_42", &id) && id == 42);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("udv_0", &id) && id == 0);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("udv_1", &id) && id == 1);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("udv_777", &id) && id == 777);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("udv_udv_777", &id) && id == 777);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("udv_udv1_udv_777", &id) && id == 777);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("queue2_776", &id) && id == 776);
  ASSERT_TRUE(iptv_cloud::stream::GetElementId("udv_24_udv_42", &id) && id == 42);
}

TEST(element_id_t, GetPadId) {
  int id;
  ASSERT_FALSE(iptv_cloud::stream::GetPadId("udv_", nullptr));
  ASSERT_FALSE(iptv_cloud::stream::GetPadId("udv_", &id));
  ASSERT_FALSE(iptv_cloud::stream::GetPadId("udv_42_udv", &id));
  ASSERT_FALSE(iptv_cloud::stream::GetPadId("udv42", &id));
  ASSERT_FALSE(iptv_cloud::stream::GetPadId("_42", &id) && id == 42);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("udv_0", &id) && id == 0);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("udv_1", &id) && id == 1);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("udv_777", &id) && id == 777);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("udv_udv_777", &id) && id == 777);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("udv_udv1_udv_777", &id) && id == 777);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("queue2_776", &id) && id == 776);
  ASSERT_TRUE(iptv_cloud::stream::GetPadId("udv_24_udv_42", &id) && id == 42);
}

TEST(m3u8, GetIndexFromHttpTsTemplate) {
  uint64_t ind;
  ASSERT_TRUE(iptv_cloud::stream::GetIndexFromHttpTsTemplate("123_324.ts", &ind));
  ASSERT_EQ(ind, 324);

  uint64_t ind2;
  ASSERT_TRUE(iptv_cloud::stream::GetIndexFromHttpTsTemplate("123_0.ts", &ind2));
  ASSERT_EQ(ind2, 0);

  uint64_t ind3;
  ASSERT_FALSE(iptv_cloud::stream::GetIndexFromHttpTsTemplate("123_g.ts", &ind3));
}
