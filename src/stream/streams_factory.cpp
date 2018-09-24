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

#include "stream/streams_factory.h"

#include "stream/constants.h" // for TYPE_FIELD, INPUT_FIELD, TIMESHIFT_DIR_FIELD

#include "stream/streams/catchup_stream.h"
#include "stream/streams/device_stream.h"
#include "stream/streams/encoding_only_audio_stream.h"
#include "stream/streams/encoding_only_video_stream.h"
#include "stream/streams/mosaic_stream.h"
#include "stream/streams/playlist_encoding_stream.h"
#include "stream/streams/playlist_relay_stream.h"
#include "stream/streams/test_stream.h"
#include "stream/streams/timeshift_player_stream.h"
#include "stream/streams/timeshift_recorder_stream.h"

#include "configs_factory.h"

#include "utils/arg_converter.h"

namespace iptv_cloud {
namespace stream {

namespace {
TimeShiftInfo make_timeshift_info(const utils::ArgsMap &args) {
  TimeShiftInfo tinfo;

  std::string timeshift_dir;
  if (!utils::ArgsGetValue(args, TIMESHIFT_DIR_FIELD, &timeshift_dir)) {
    CRITICAL_LOG() << "Define " TIMESHIFT_DIR_FIELD
                      " variable and make it valid.";
  }
  tinfo.timshift_dir =
      common::file_system::ascii_directory_string_path(timeshift_dir);

  tinfo.chunk_max_life_time_hours = 0;
  if (!utils::ArgsGetValue(args, TIMESHIFT_CHUNK_MAX_LIFE_TIME_FIELD,
                           &tinfo.chunk_max_life_time_hours)) {
    tinfo.chunk_max_life_time_hours = 12;
  }

  time_shift_delay_t timeshift_delay = 0;
  if (utils::ArgsGetValue(args, TIMESHIFT_DELAY_FIELD, &timeshift_delay)) {
    tinfo.timeshift_delay = timeshift_delay;
  }
  return tinfo;
}

} // namespace

IBaseStream *StreamsFactory::CreateStream(const utils::ArgsMap &args,
                                          IBaseStream::IStreamClient *client,
                                          StreamStruct *stats,
                                          chunk_index_t start_chunk_index) {
  Config *config = make_config(args);
  input_t input = config->GetInput();
  if (config->GetType() == RELAY) {
    streams::RelayConfig *rconfig = static_cast<streams::RelayConfig *>(config);
    InputUri iuri = input[0];
    common::uri::Url input_uri = iuri.GetInput();
    if (input_uri.GetScheme() ==
        common::uri::Url::file) { // multi input or playlist
      streams::PlaylistRelayConfig *prconfig =
          static_cast<streams::PlaylistRelayConfig *>(config);
      return new streams::PlaylistRelayStream(prconfig, client, stats);
    }

    /*if (input.size() > 1) {
      return new streams::MosaicStream(rconfig, client, stats);
    }*/

    return new streams::RelayStream(rconfig, client, stats);
  } else if (config->GetType() == ENCODING) {
    streams::EncodingConfig *econfig =
        static_cast<streams::EncodingConfig *>(config);
    InputUri iuri = input[0];
    common::uri::Url input_uri = iuri.GetInput();
    if (input_uri.GetScheme() ==
        common::uri::Url::file) { // multi input or playlist
      return new streams::PlaylistEncodingStream(econfig, client, stats);
    }

    if (input.size() > 1) {
      return new streams::MosaicStream(econfig, client, stats);
    }

    if (IsTestUrl(iuri)) {
      return new streams::TestStream(econfig, client, stats);
    }

    if (input_uri.GetScheme() == common::uri::Url::dev) {
      return new streams::DeviceStream(econfig, client, stats);
    }

    if (iuri.GetRelayVideo()) {
      return new streams::EncodingOnlyAudioStream(econfig, client, stats);
    } else if (iuri.GetRelayAudio()) {
      return new streams::EncodingOnlyVideoStream(econfig, client, stats);
    }

    return new streams::EncodingStream(econfig, client, stats);
  } else if (config->GetType() == TIMESHIFT_PLAYER) {
    streams::RelayConfig *tconfig = static_cast<streams::RelayConfig *>(config);
    TimeShiftInfo tinfo = make_timeshift_info(args);
    return new streams::TimeShiftPlayerStream(tconfig, tinfo, client, stats,
                                              start_chunk_index);
  } else if (config->GetType() == TIMESHIFT_RECORDER) {
    streams::TimeshiftConfig *tconfig =
        static_cast<streams::TimeshiftConfig *>(config);
    TimeShiftInfo tinfo = make_timeshift_info(args);
    return new streams::TimeShiftRecorderStream(tconfig, tinfo, client, stats);
  } else if (config->GetType() == CATCHUP) {
    streams::TimeshiftConfig *tconfig =
        static_cast<streams::TimeshiftConfig *>(config);
    TimeShiftInfo tinfo = make_timeshift_info(args);
    return new streams::CatchupStream(tconfig, tinfo, client, stats);
  }

  NOTREACHED();
  return nullptr;
}

bool IsTimeshiftPlayer(const utils::ArgsMap &args, TimeShiftInfo *tinfo) {
  if (!tinfo) {
    return false;
  }

  std::string type_str;
  if (!utils::ArgsGetValue(args, TYPE_FIELD, &type_str)) {
    CRITICAL_LOG() << "Define " TYPE_FIELD " variable and make it valid.";
  }

  StreamType type;
  if (!common::ConvertFromString(type_str, &type)) {
    return false;
  }

  if (type != TIMESHIFT_PLAYER) {
    return false;
  }

  *tinfo = make_timeshift_info(args);
  input_t input;
  if (!read_input(args, &input)) {
    CRITICAL_LOG() << "Define " INPUT_FIELD " variable and make it valid.";
  }

  return true;
}

} // namespace stream
} // namespace iptv_cloud
