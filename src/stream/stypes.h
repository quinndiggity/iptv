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

#include <map>
#include <string> // for string

#include <common/uri/url.h>

#define TS_DURATION 10

#define VIDEO_TEE_NAME_1U "video_tee_%lu"
#define AUDIO_TEE_NAME_1U "audio_tee_%lu"

#define UDB_VIDEO_NAME_1U "udb_conn_video_%lu"
#define UDB_AUDIO_NAME_1U "udb_conn_audio_%lu"

#define POST_PROC_NAME_1U "post_proc_%lu"
#define VIDEO_LOGO_NAME_1U "videologo_%lu"
#define DEINTERLACE_NAME_1U "deinterlace_%lu"
#define ASPECT_RATIO_NAME_1U "aspect_ratio_%lu"
#define VIDEO_CONVERT_NAME_1U "video_convert_%lu"
#define ENCODER_VIDEO_CONVERT_NAME_1U "encoder_video_convert_%lu"
#define AUTO_VIDEO_CONVERT_NAME_1U "auto_video_convert_%lu"
#define AUDIO_CONVERT_NAME_1U "audio_convert_%lu"
#define VOLUME_NAME_1U "volume_%lu"

#define VIDEOMIXER_NAME_1U "videomixer_%lu"
#define INTERLIVE_NAME_1U "interlive_%lu"
#define CAIRO_NAME_1U "cairo_%lu"
#define QUEUE2_NAME_1U "queue2_%lu"

#define AUDIO_CONVERT_CAPS_FILTER_NAME_1U "audio_convert_capsfilter_%lu"

#define SRC_NAME_1U "src_%lu"

#define VIDEO_CODEC_NAME_1U "video_codec_%lu"
#define AUDIO_CODEC_NAME_1U "audio_codec_%lu"

#define VIDEO_CAPS_DEVICE_NAME_1U "video_caps_device_%lu"
#define VIDEO_SCALE_CAPS_FILTER_NAME_1U "videoscale_capsfilter_%lu"
#define VIDEO_X264ENC_CAPS_FILTER_STREAM_FORMAT_NAME_1U                        \
  "x264enc_capsfilter_stream_format_%lu"
#define VIDEO_X264ENC_CAPS_FILTER_PROFILE_NAME_1U                              \
  "x264enc_capsfilter_profile_%lu"
#define VIDEO_SCALE_NAME_1U "videoscale_%lu"
#define VIDEO_BOX_NAME_1U "videobox_%lu"
#define VIDEO_RATE_CAPS_FILTER_NAME_1U "videorate_capsfilter_%lu"
#define VIDEO_RATE_NAME_1U "videorate_%lu"

#define AUDIO_RESAMPLE_NAME_1U "audioresample_%lu"
#define MPEG_AUDIO_PARSE_NAME_1U "mpegaudioparse_%lu"

#define DECODEBIN_NAME_1U "decodebin_%lu"
#define VIDEOBOX_NAME_1U "videobox_%lu"

#define VIDEO_DECODEBIN_NAME_1U "video_decodebin_%lu"
#define AUDIO_DECODEBIN_NAME_1U "audio_decodebin_%lu"

#define SINK_NAME_1U "sink_%lu"
#define AUDIO_SINK_NAME_1U "audio_sink_%lu"
#define VIDEO_SINK_NAME_1U "video_sink_%lu"

#define AUDIO_PARSER_NAME_1U "audio_parser_%lu"
#define VIDEO_PARSER_NAME_1U "video_parser_%lu"
#define MUXER_NAME_1U "muxer_%lu"

#define AUDIO_PAY_NAME_1U "audio_pay_%lu"
#define VIDEO_PAY_NAME_1U "video_pay_%lu"

#define VIDEO_TEE_QUEUE_NAME_1U "video_tee_queue_%lu"
#define AUDIO_TEE_QUEUE_NAME_1U "audio_tee_queue_%lu"

#define AUDIO_LEVEL_NAME_1U "level_%lu"

#define CHUNK_EXT ".ts"

#define TS_TEMPLATE "%05d" CHUNK_EXT

// devices
#define SCREEN_URL "screen"
#define DECKLINK_URL "decklink"

#define RECORDING_URL "rec"
#define FAKE_URL "fake"

namespace iptv_cloud {
namespace stream {

typedef size_t element_id_t;
typedef int audio_channel_count_t;
typedef double alpha_t;
typedef uint8_t decklink_video_mode_t;

typedef std::map<std::string, uint32_t> video_encoders_args_t;
typedef std::map<std::string, std::string> video_encoders_str_args_t;

bool GetElementId(const std::string &name, element_id_t *elem_id);
bool GetPadId(const std::string &name, int *pad_id);

enum SinkDeviceType { SCREEN_OUTPUT, DECKLINK_OUTPUT };

enum SupportedOtherType {
  APPLICATION_HLS_TYPE,      // "application/x-hls"
  APPLICATION_ICY_TYPE,      // "application/x-icy"
  APPLICATION_TELETEXT_TYPE, // "application/x-teletext"
  APPLICATION_GZIP_TYPE,     // "application/x-gzip"
  SUBPICTURE_DVB_TYPE,       // "subpicture/x-dvb"
  IMAGE_PNG_TYPE,            // "image/png"
  IMAGE_JPEG_TYPE            // "image/jpeg"
};

enum SupportedDemuxers {
  VIDEO_MPEGTS_DEMUXER, // "video/mpegts"
  VIDEO_FLV_DEMUXER     // "video/x-flv"
};

enum SupportedVideoCodecs {
  VIDEO_H264_CODEC, // video/x-h264
  VIDEO_MPEG_CODEC, // video/mpeg
  VIDEO_H265_CODEC  // video/x-h265
};

enum SupportedAudioCodecs {
  AUDIO_MPEG_CODEC, // "audio/mpeg"
  AUDIO_AAC_CODEC,  // "audio/aac"
  AUDIO_AC3_CODEC   // "audio/x-ac3"
};

enum SupportedRawStreams {
  VIDEO_RAW_STREAM, // video/x-raw
  AUDIO_RAW_STREAM  // audio/x-raw
};

bool IsOtherFromType(const std::string &type, SupportedOtherType *oc);
bool IsDemuxerFromType(const std::string &type, SupportedDemuxers *dc);
bool IsVideoCodecFromType(const std::string &type, SupportedVideoCodecs *vc);
bool IsAudioCodecFromType(const std::string &type, SupportedAudioCodecs *ac);
bool IsRawStreamFromType(const std::string &type, SupportedRawStreams *rc);

std::string GenHttpTsTemplate(time_t msec);
bool GetIndexFromHttpTsTemplate(const std::string &file_name, uint64_t *index);

bool IsScreenUrl(const common::uri::Url &url);
bool IsDecklinkUrl(const common::uri::Url &url);
bool IsDeviceOutUrl(const common::uri::Url &url, SinkDeviceType *type);

bool IsRecordingUrl(const common::uri::Url &url);
bool IsFakeUrl(const common::uri::Url &url);

} // namespace stream
} // namespace iptv_cloud
