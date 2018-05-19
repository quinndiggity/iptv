#include <unistd.h>

#include <thread>

#include <gst/gst.h>

#include <common/file_system/file_system.h>
#include <common/sprintf.h>

#include "stream/constants.h"

#include "stream/configs_factory.h"
#include "stream/streams/screen_stream.h"
#include "stream/streams_factory.h"

#define STREAMS_TRY_COUNT 10

#define SCREEN_SLEEP_SEC 10
#define RELAY_SLEEP_SEC 10
#define RELAY_PLAYLIST_SLEEP_SEC 10
#define ENCODE_SLEEP_SEC 10
#define TIMESHIFT_REC_SLEEP_SEC 10
#define TIMESHIFT_PLAYLIST_SLEEP_SEC 10
#define MOSAIC_SLEEP_SEC 10
#define AD_SLEEP_SEC 10
#define RELAY_AUDIO_SLEEP_SEC 10
#define RELAY_VIDEO_SLEEP_SEC 10
#define STATIC_IMAGE_SLEEP_SEC 10
#define TEST_SLEEP_SEC 10
#define CATCHUP_SLEEP_SEC 20
#define M3U8LOG_SLEEP_SEC 10
#define DIFF_SEC 1

#define INPUT_URI "http://4bf17cc1.ottclub.ru/iptv/W2AC7MQR8NAYK4/160/index.m3u8"
#define RELAY_OUTPUT_URI "rtmp://4.31.30.153:1935/devapp/test_r"
#define ENCODING_OUTPUT_URI "rtmp://4.31.30.153:1935/devapp/test_e"
#define TIMESHIFT_OUTPUT_URI "rtmp://4.31.30.153:1935/devapp/test_t"
#define PLAYLIST_OUTPUT_URI "rtmp://4.31.30.153:1935/devapp/test_p"
#define PLAYLIST_INPUT_FILE_URI "file://" PROJECT_TEST_SOURCES_DIR "/test.ts"
#define STATIC_IMAGE_URI "file://" PROJECT_TEST_SOURCES_DIR "/static.png"
#define M3U8LOG_INPUT_URI "http://inghls-i.akamaihd.net/hls/live/494828/oranews/log.m3u8"
#define M3U8LOG_OUTPUT_URI "http://post.inghls-i.akamaihd.net/494828/oranewsasd/log_1h.m3u8"

void* quit_job(iptv_cloud::stream::IBaseStream* job, uint32_t sleep_sec) {
  uint32_t sl = rand() % sleep_sec + DIFF_SEC;
  sleep(sl);
  job->Quit(iptv_cloud::stream::EXIT_SELF);
  return NULL;
}

void check_screen() {
  iptv_cloud::output_t ouris;
  iptv_cloud::OutputUri ouri(0, common::uri::Url(SCREEN_URL));
  ouris.push_back(ouri);

  iptv_cloud::input_t input;
  iptv_cloud::InputUri iuri(0, common::uri::Url(SCREEN_URL));
  input.push_back(iuri);
  const std::string iuri_str = common::ConvertToString(input);
  const std::string ouri_str = common::ConvertToString(ouris);
  const iptv_cloud::utils::ArgsMap args_screen = {
      std::make_pair(ID_FIELD, "screen"), std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::SCREEN)),
      std::make_pair(FEEDBACK_DIR_FIELD, "~"), std::make_pair(INPUT_FIELD, iuri_str),
      std::make_pair(OUTPUT_FIELD, ouri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct screen(
        iptv_cloud::StreamInfo{common::MemSPrintf("screen_%llu", i), iptv_cloud::SCREEN, {0}, {0}});
    iptv_cloud::stream::IBaseStream* job_screen = new iptv_cloud::stream::streams::ScreenStream(
        static_cast<iptv_cloud::stream::streams::AudioVideoConfig*>(iptv_cloud::stream::make_config(args_screen)),
        nullptr, &screen);
    std::thread th(quit_job, job_screen, SCREEN_SLEEP_SEC);
    CHECK(job_screen->GetType() == iptv_cloud::SCREEN);
    job_screen->Exec();
    th.join();
    delete job_screen;
  }
}

void check_relay(const iptv_cloud::input_t& input) {
  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::OutputUri oruri(0, common::uri::Url(RELAY_OUTPUT_URI));
  iptv_cloud::output_t ouri2;
  ouri2.push_back(oruri);
  const std::string remux_uri_str = common::ConvertToString(ouri2);
  const iptv_cloud::utils::ArgsMap args_remux = {
      std::make_pair(ID_FIELD, "remux"), std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::RELAY)),
      std::make_pair(FEEDBACK_DIR_FIELD, "~"), std::make_pair(INPUT_FIELD, iuri_str),
      std::make_pair(OUTPUT_FIELD, remux_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct remux(
        iptv_cloud::StreamInfo{common::MemSPrintf("remux_%llu", i), iptv_cloud::RELAY, {0}, {0}});
    iptv_cloud::stream::IBaseStream* job_remux = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args_remux, nullptr, &remux, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, job_remux, RELAY_SLEEP_SEC);
    CHECK(job_remux->GetType() == iptv_cloud::RELAY);
    job_remux->Exec();
    th.join();
    delete job_remux;
  }
}

void check_relay_playlist() {
  iptv_cloud::input_t input;
  iptv_cloud::InputUri iuri(0, common::uri::Url(PLAYLIST_INPUT_FILE_URI));
  input.push_back(iuri);
  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::OutputUri ouri(0, common::uri::Url(RELAY_OUTPUT_URI));
  iptv_cloud::output_t output;
  output.push_back(ouri);
  const std::string relay_playlist_uri_str = common::ConvertToString(output);
  const iptv_cloud::utils::ArgsMap args_relay = {std::make_pair(ID_FIELD, "relay_playlist"),
                                                 std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::RELAY)),
                                                 std::make_pair(INPUT_FIELD, iuri_str),
                                                 std::make_pair(OUTPUT_FIELD, relay_playlist_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct relay_playlist(
        iptv_cloud::StreamInfo{common::MemSPrintf("relay_playlist_%llu", i), iptv_cloud::RELAY, {0}, {0}});
    iptv_cloud::stream::IBaseStream* job_relay_playlist =
        iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(args_relay, nullptr, &relay_playlist,
                                                                       iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, job_relay_playlist, RELAY_PLAYLIST_SLEEP_SEC);
    CHECK(job_relay_playlist->GetType() == iptv_cloud::RELAY);
    job_relay_playlist->Exec();
    th.join();
    delete job_relay_playlist;
  }
}

void check_encoding(const iptv_cloud::input_t& input) {
  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::output_t ouri3;
  iptv_cloud::OutputUri oeuri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  ouri3.push_back(oeuri);
  const std::string enc_uri_str = common::ConvertToString(ouri3);
  const iptv_cloud::utils::ArgsMap args_enc = {
      std::make_pair(ID_FIELD, "encoding"),
      std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
      std::make_pair(FEEDBACK_DIR_FIELD, "~"),
      std::make_pair(VIDEO_CODEC_FIELD, "x264enc"),
      std::make_pair(INPUT_FIELD, iuri_str),
      std::make_pair(OUTPUT_FIELD, enc_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct encoding(
        iptv_cloud::StreamInfo{common::MemSPrintf("encoding_%llu", i), iptv_cloud::ENCODING, {0}, {0}});
    iptv_cloud::stream::IBaseStream* job_enc = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args_enc, nullptr, &encoding, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, job_enc, ENCODE_SLEEP_SEC);
    CHECK(job_enc->GetType() == iptv_cloud::ENCODING);
    job_enc->Exec();
    th.join();
    delete job_enc;
  }
}

void check_timeshift(const iptv_cloud::input_t& input) {
  const std::string iuri_str = common::ConvertToString(input);

  const iptv_cloud::utils::ArgsMap args_timeshift = {
      std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::TIMESHIFT_RECORDER)),
      std::make_pair(TIMESHIFT_DIR_FIELD, "/tmp/"), std::make_pair(FEEDBACK_DIR_FIELD, "~"),
      std::make_pair(VIDEO_CODEC_FIELD, "x264enc")};
  iptv_cloud::utils::ArgsMap args_rec = args_timeshift;
  args_rec.push_back(std::make_pair(ID_FIELD, "timeshift_rec"));
  args_rec.push_back(std::make_pair(INPUT_FIELD, iuri_str));
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct timeshift_rec(
        iptv_cloud::StreamInfo{common::MemSPrintf("timeshift_rec_%llu", i), iptv_cloud::TIMESHIFT_RECORDER, {0}, {}});
    iptv_cloud::stream::IBaseStream* job_timeshift_rec = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args_rec, nullptr, &timeshift_rec, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, job_timeshift_rec, TIMESHIFT_REC_SLEEP_SEC);
    CHECK(job_timeshift_rec->GetType() == iptv_cloud::TIMESHIFT_RECORDER);
    job_timeshift_rec->Exec();
    th.join();
    delete job_timeshift_rec;
  }

  iptv_cloud::output_t iuris;
  iptv_cloud::OutputUri iuri(0, common::uri::Url());
  iuris.push_back(iuri);

  iptv_cloud::output_t ouri3;
  iptv_cloud::OutputUri oeuri(0, common::uri::Url(TIMESHIFT_OUTPUT_URI));
  ouri3.push_back(oeuri);
  const std::string enc_uri_str = common::ConvertToString(ouri3);
  iptv_cloud::utils::ArgsMap args_play = args_timeshift;
  args_rec.push_back(std::make_pair(ID_FIELD, "timeshift_rplay"));
  args_play.push_back(std::make_pair(INPUT_FIELD, common::ConvertToString(iuris)));
  args_play.push_back(std::make_pair(OUTPUT_FIELD, enc_uri_str));
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct timeshift_play(
        iptv_cloud::StreamInfo{common::MemSPrintf("timeshift_play_%llu", i), iptv_cloud::TIMESHIFT_PLAYER, {0}, {0}});
    iptv_cloud::stream::IBaseStream* job_timeshift_play =
        iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(args_play, nullptr, &timeshift_play, 0);
    std::thread th(quit_job, job_timeshift_play, TIMESHIFT_REC_SLEEP_SEC);
    CHECK(job_timeshift_play->GetType() == iptv_cloud::TIMESHIFT_PLAYER);
    job_timeshift_play->Exec();
    th.join();
    delete job_timeshift_play;
  }
}

void check_catchup(const iptv_cloud::input_t& input) {
  const std::string iuri = common::ConvertToString(input);

  const std::string test_dir = "/tmp/catchup_test";
  const iptv_cloud::utils::ArgsMap args = {std::make_pair(ID_FIELD, "catchup"),
                                           std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::CATCHUP)),
                                           std::make_pair(AUTO_EXIT_TIME_FIELD, "15"),
                                           std::make_pair(TIMESHIFT_CHUNK_DURATION_FIELD, "5"),
                                           std::make_pair(TIMESHIFT_DIR_FIELD, test_dir),
                                           std::make_pair(INPUT_FIELD, iuri)};

  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    common::ErrnoError err = common::file_system::create_directory(test_dir, true);
    CHECK(!err);
    iptv_cloud::StreamStruct jobstr(
        iptv_cloud::StreamInfo{common::MemSPrintf("catchup_%llu", i), iptv_cloud::CATCHUP, {0}, {}});
    iptv_cloud::stream::IBaseStream* catchup_job = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args, nullptr, &jobstr, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, catchup_job, CATCHUP_SLEEP_SEC);
    CHECK(catchup_job->GetType() == iptv_cloud::CATCHUP);
    catchup_job->Exec();
    th.join();
    delete catchup_job;
    err = common::file_system::remove_directory(test_dir, true);
    CHECK(!err);
  }
}

void check_playlist() {
  iptv_cloud::input_t input;
  iptv_cloud::InputUri iuri0(0, common::uri::Url(PLAYLIST_INPUT_FILE_URI));
  iptv_cloud::InputUri iuri1(1, common::uri::Url(PLAYLIST_INPUT_FILE_URI));
  input.push_back(iuri0);
  input.push_back(iuri1);

  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::output_t ouri3;
  iptv_cloud::OutputUri oeuri(0, common::uri::Url(PLAYLIST_OUTPUT_URI));
  ouri3.push_back(oeuri);
  const std::string playlist_uri_str = common::ConvertToString(ouri3);
  const iptv_cloud::utils::ArgsMap args_enc = {
      std::make_pair(ID_FIELD, "playlist"),
      std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
      std::make_pair(FEEDBACK_DIR_FIELD, "~"),
      std::make_pair(VIDEO_CODEC_FIELD, "x264enc"),
      std::make_pair(INPUT_FIELD, iuri_str),
      std::make_pair(OUTPUT_FIELD, playlist_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct playlist(
        iptv_cloud::StreamInfo{common::MemSPrintf("playlist_%llu", i), iptv_cloud::ENCODING, {0, 1}, {0}});
    iptv_cloud::stream::IBaseStream* job_playlist = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args_enc, nullptr, &playlist, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, job_playlist, TIMESHIFT_PLAYLIST_SLEEP_SEC);
    CHECK(job_playlist->GetType() == iptv_cloud::ENCODING);
    job_playlist->Exec();
    th.join();
    delete job_playlist;
  }
}

void check_mosaic() {
  iptv_cloud::input_t input;
  iptv_cloud::InputUri iuri(0, common::uri::Url(INPUT_URI));
  input.push_back(iuri);
  input.push_back(iuri);
  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::output_t ouri;
  iptv_cloud::OutputUri omuri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  ouri.push_back(omuri);
  const std::string mos_uri_str = common::ConvertToString(ouri);
  const iptv_cloud::utils::ArgsMap args = {
      std::make_pair(ID_FIELD, "mosaic"), std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
      std::make_pair(INPUT_FIELD, iuri_str), std::make_pair(OUTPUT_FIELD, mos_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct mosaic(
        iptv_cloud::StreamInfo{common::MemSPrintf("mosaic_%llu", i), iptv_cloud::ENCODING, {0, 1}, {0}});
    iptv_cloud::stream::IBaseStream* mosaic_job = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args, nullptr, &mosaic, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, mosaic_job, MOSAIC_SLEEP_SEC);
    CHECK(mosaic_job->GetType() == iptv_cloud::ENCODING);
    mosaic_job->Exec();
    th.join();
    delete mosaic_job;
  }
}

void check_relay_audio() {
  const std::string iuri_str =
      "{\"urls\":[ "
      "{\"id\":148,\"uri\":\"http://1-fr.cdn.siqnal.tv/56e8230881ea8ef80c88750c/BHT1.m3u8\", "
      "\"relay_audio\":true} ] }";

  iptv_cloud::output_t output;
  iptv_cloud::OutputUri ouri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  output.push_back(ouri);
  const std::string relay_audio_uri_str = common::ConvertToString(output);
  const iptv_cloud::utils::ArgsMap args = {std::make_pair(ID_FIELD, "relay_audio"),
                                           std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
                                           std::make_pair(INPUT_FIELD, iuri_str),
                                           std::make_pair(OUTPUT_FIELD, relay_audio_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct relay_audio(
        iptv_cloud::StreamInfo{common::MemSPrintf("relay_audio_%llu", i), iptv_cloud::ENCODING, {0}, {0}});
    iptv_cloud::stream::IBaseStream* relay_audio_job = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args, nullptr, &relay_audio, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, relay_audio_job, RELAY_AUDIO_SLEEP_SEC);
    CHECK(relay_audio_job->GetType() == iptv_cloud::ENCODING);
    relay_audio_job->Exec();
    th.join();
    delete relay_audio_job;
  }
}

void check_relay_video() {
  const std::string iuri_str =
      "{\"urls\":[ "
      "{\"id\":148,\"uri\":\"http://1-fr.cdn.siqnal.tv/56e8230881ea8ef80c88750c/BHT1.m3u8\", "
      "\"relay_video\":true} ] }";

  iptv_cloud::output_t output;
  iptv_cloud::OutputUri ouri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  output.push_back(ouri);
  const std::string relay_video_uri_str = common::ConvertToString(output);
  const iptv_cloud::utils::ArgsMap args = {std::make_pair(ID_FIELD, "relay_video"),
                                           std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
                                           std::make_pair(INPUT_FIELD, iuri_str),
                                           std::make_pair(OUTPUT_FIELD, relay_video_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct relay_video(
        iptv_cloud::StreamInfo{common::MemSPrintf("relay_video_%llu", i), iptv_cloud::ENCODING, {0}, {0}});
    iptv_cloud::stream::IBaseStream* relay_video_job = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args, nullptr, &relay_video, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, relay_video_job, RELAY_VIDEO_SLEEP_SEC);
    CHECK(relay_video_job->GetType() == iptv_cloud::ENCODING);
    relay_video_job->Exec();
    th.join();
    delete relay_video_job;
  }
}

void check_static_image() {
  const std::string iuri_str = "{\"urls\":[{\"id\":150,\"static_image\":\"" STATIC_IMAGE_URI
                               "\",\"uri\":\"http://ice31.securenetsystems.net/AL09\"}]}";

  iptv_cloud::output_t output;
  iptv_cloud::OutputUri ouri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  output.push_back(ouri);
  const std::string static_image_uri_str = common::ConvertToString(output);
  const iptv_cloud::utils::ArgsMap args = {std::make_pair(ID_FIELD, "static_image"),
                                           std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
                                           std::make_pair(INPUT_FIELD, iuri_str),
                                           std::make_pair(OUTPUT_FIELD, static_image_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct static_image(
        iptv_cloud::StreamInfo{common::MemSPrintf("static_image_%llu", i), iptv_cloud::ENCODING, {0}, {0}});
    iptv_cloud::stream::IBaseStream* static_image_job = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args, nullptr, &static_image, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, static_image_job, STATIC_IMAGE_SLEEP_SEC);
    CHECK(static_image_job->GetType() == iptv_cloud::ENCODING);
    static_image_job->Exec();
    th.join();
    delete static_image_job;
  }
}

void check_ad_feature(const iptv_cloud::input_t& input) {
  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::output_t ouri3;
  iptv_cloud::OutputUri oeuri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  ouri3.push_back(oeuri);
  const std::string enc_uri_str = common::ConvertToString(ouri3);
  const iptv_cloud::utils::ArgsMap args_enc = {
      std::make_pair(ID_FIELD, "ad"), std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
      std::make_pair(INPUT_FIELD, iuri_str), std::make_pair(OUTPUT_FIELD, enc_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct encoding(
        iptv_cloud::StreamInfo{common::MemSPrintf("ad_%llu", i), iptv_cloud::ENCODING, {0}, {0}});
    iptv_cloud::stream::IBaseStream* job_enc = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args_enc, nullptr, &encoding, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, job_enc, AD_SLEEP_SEC);
    CHECK(job_enc->GetType() == iptv_cloud::ENCODING);
    job_enc->Exec();
    th.join();
    delete job_enc;
  }
}

void check_test() {
  iptv_cloud::input_t input;
  iptv_cloud::InputUri iuri(0, common::uri::Url(TEST_URL));
  input.push_back(iuri);
  const std::string iuri_str = common::ConvertToString(input);

  iptv_cloud::output_t output;
  iptv_cloud::OutputUri ouri(0, common::uri::Url(ENCODING_OUTPUT_URI));
  output.push_back(ouri);
  const std::string test_uri_str = common::ConvertToString(output);
  const iptv_cloud::utils::ArgsMap args = {
      std::make_pair(ID_FIELD, "test"), std::make_pair(TYPE_FIELD, common::ConvertToString(iptv_cloud::ENCODING)),
      std::make_pair(INPUT_FIELD, iuri_str), std::make_pair(OUTPUT_FIELD, test_uri_str)};
  for (size_t i = 0; i < STREAMS_TRY_COUNT; ++i) {
    iptv_cloud::StreamStruct test(
        iptv_cloud::StreamInfo{common::MemSPrintf("test_%llu", i), iptv_cloud::ENCODING, {0}, {0}});
    iptv_cloud::stream::IBaseStream* test_job = iptv_cloud::stream::StreamsFactory::GetInstance().CreateStream(
        args, nullptr, &test, iptv_cloud::stream::invalid_chunk_index);
    std::thread th(quit_job, test_job, TEST_SLEEP_SEC);
    CHECK(test_job->GetType() == iptv_cloud::ENCODING);
    test_job->Exec();
    th.join();
    delete test_job;
  }
}

int main(int argc, char** argv) {
  iptv_cloud::stream::streams_init(argc, argv);

  iptv_cloud::input_t iuris;
  iptv_cloud::InputUri iuri(0, common::uri::Url(INPUT_URI));
  iuris.push_back(iuri);

  // check_playlist();
  check_screen();
  check_relay(iuris);
  check_relay_playlist();
  check_encoding(iuris);
  check_timeshift(iuris);
  check_catchup(iuris);
  check_mosaic();
  check_ad_feature(iuris);
  check_relay_audio();
  check_relay_video();
  check_static_image();
  check_test();

  return 0;
}
