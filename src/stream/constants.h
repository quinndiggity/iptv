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

#include <common/media/types.h>

#define TS_EXTENSION "ts"
#define M3U8_EXTENSION "m3u8"

#define M3U8_CHUNK_MARKER "#EXTINF"

// cmd line
#define CONFIG_FIELD "config"
#define CONFIG_DATA_FIELD "config_data"
#define FEEDBACK_DIR_FIELD "feedback_dir" // required
#define LOG_LEVEL_FIELD "log_level"

#define DEFAULT_VIDEO_BITRATE 0
#define DEFAULT_AUDIO_BITRATE 0
#define DEFAULT_VOLUME 1
#define DEFAULT_AUDIO_CHANNEL_COUNT 0
#define DEFAULT_FRAME_RATE 0
#define DEFAULT_VIDEO_HEIGHT 0
#define DEFAULT_VIDEO_WIDTH 0
#define DEFAULT_LOGO_ALPHA 1
#define DEFAULT_VIDEO_PARSER "h264parse"
#define DEFAULT_AUDIO_PARSER "aacparse"
#define DEFAULT_VIDEO_ENCODER "x264enc"
#define DEFAULT_AUDIO_ENCODER "faac"
#define DEFAULT_AUDIO_SELECT -1
#define DEFAULT_DECKLINK_VIDEO_MODE 1

#define DEFAULT_ASPECT_RATIO_NUM 0
#define DEFAULT_ASPECT_RATIO_DEN 0

#define DEFAULT_TIMESHIFT_CHUNK_DURATION 120

#define ID_FIELD "id"     // required for daemon and slave
#define TYPE_FIELD "type" // required for slave
#define STATS_CREDENTIALS_FIELD "stats_credentials" // required for daemon
#define LOG_FILE_FIELD "log_file"
#define AUTO_EXIT_TIME_FIELD "auto_exit_time"

#define INPUT_FIELD "input" // required
#define OUTPUT_FIELD "output"
#define NO_VIDEO_FIELD "no_video"
#define NO_AUDIO_FIELD "no_audio"
#define DEINTERLACE_FIELD "deinterlace"
#define FRAME_RATE_FIELD "framerate"
#define AUDIO_CHANNELS_FIELD "audio_channels"
#define VOLUME_FIELD "volume"
#define VIDEO_PARSER_FIELD "video_parser"
#define AUDIO_PARSER_FIELD "audio_parser"
#define VIDEO_CODEC_FIELD "video_codec"
#define AUDIO_CODEC_FIELD "audio_codec"
#define AUDIO_SELECT_FIELD "audio_select"
#define TIMESHIFT_DIR_FIELD "timeshift_dir" // requeired in timeshift mode
#define TIMESHIFT_CHUNK_MAX_LIFE_TIME_FIELD "chunk_max_life_time" // in hours
#define TIMESHIFT_DELAY_FIELD "timeshift_delay"                   // in mins
#define TIMESHIFT_CHUNK_DURATION_FIELD "timeshift_chunk_duration"
#define LOGO_PATH_FIELD "logo_path"
#define LOGO_POS_FIELD "logo_pos"
#define LOGO_ALPHA_FIELD "logo_alpha"
#define LOOP_FIELD "loop"
#define RESTART_ATTEMPTS_FIELD "restart_attempts"
#define DELAY_TIME_FIELD "delay_time"
#define WIDTH_FIELD "width"
#define HEIGHT_FIELD "height"
#define VIDEO_BITRATE_FIELD "video_bitrate"
#define AUDIO_BITRATE_FIELD "audio_bitrate"
#define MAIN_PROFILE_FIELD "mainprofile"
#define MAIN_PROFILE_EXTERNAL_FIELD "mainprofile_external"
#define ASPECT_RATIO_FIELD "aspect_ratio"

#define DECKLINK_VIDEO_MODE_FILELD "decklink_video_mode"

extern const common::media::Rational unknown_aspect_ratio;
