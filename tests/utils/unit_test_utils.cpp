#include <gtest/gtest.h>

#include "utils/chunk_info.h"

#define TEST_PLAYLIST PROJECT_TEST_SOURCES_DIR "/playlist.m3u8"
#define NEW_PLAYLIST PROJECT_TEST_SOURCES_DIR "/test_write.m3u8"

TEST(ChunkInfo, double) {
  iptv_cloud::utils::ChunkInfo ch("1497615343667_segment10012.ts", 11.43 * iptv_cloud::utils::ChunkInfo::SECOND, 10012);
  ASSERT_EQ(ch.GetDurationInSecconds(), 11.43);
}
