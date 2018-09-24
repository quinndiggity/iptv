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

#include "stream/streams/builders/encoding_stream_builder.h"

#include <common/sprintf.h>

#include "stream/constants.h"

#include "stream/elements/audio/audio.h"
#include "stream/elements/encoders/audio_encoders.h"
#include "stream/elements/encoders/video_encoders.h"
#include "stream/elements/parser/audio_parsers.h"
#include "stream/elements/parser/video_parsers.h"
#include "stream/elements/sink/screen.h"
#include "stream/elements/video/video.h"

#include "stream/pad/pad.h"

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

EncodingStreamBuilder::EncodingStreamBuilder(EncodingConfig *api,
                                             SrcDecodeBinStream *observer)
    : SrcDecodeStreamBuilder(api, observer) {}

Connector EncodingStreamBuilder::BuildPostProc(Connector conn) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);

  if (conf->HaveVideo()) {
    elements_line_t video_post_line = BuildVideoPostProc(0);
    if (!video_post_line.empty()) {
      ElementLink(conn.video, video_post_line.front());
      conn.video = video_post_line.back();
    }
  }

  if (conf->HaveAudio()) {
    elements_line_t audio_post_line = BuildAudioPostProc(0);
    if (!audio_post_line.empty()) {
      ElementLink(conn.audio, audio_post_line.front());
      conn.audio = audio_post_line.back();
    }
  }

  return conn;
}

SupportedVideoCodecs EncodingStreamBuilder::GetVideoCodecType() const {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);
  const std::string vcodec = conf->GetVideoEncoder();
  if (elements::encoders::IsH264Encoder(vcodec)) {
    return VIDEO_H264_CODEC;
  } else if (vcodec == elements::encoders::ElementX265Enc::GetPluginName()) {
    return VIDEO_H265_CODEC;
  } else if (vcodec == elements::encoders::ElementMPEG2Enc::GetPluginName()) {
    return VIDEO_MPEG_CODEC;
  }

  NOTREACHED();
  return VIDEO_H264_CODEC;
}

SupportedAudioCodecs EncodingStreamBuilder::GetAudioCodecType() const {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);
  const std::string acodec = conf->GetAudioEncoder();
  if (elements::encoders::IsAACEncoder(acodec)) {
    return AUDIO_AAC_CODEC;
  } else if (acodec == elements::encoders::ElementMP3Enc::GetPluginName()) {
    return AUDIO_MPEG_CODEC;
  }

  NOTREACHED();
  return AUDIO_MPEG_CODEC;
}

Connector EncodingStreamBuilder::BuildConverter(Connector conn) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);
  if (conf->HaveVideo()) {
    elements_line_t video_encoder = BuildVideoConverter(0);
    if (!video_encoder.empty()) {
      ElementLink(conn.video, video_encoder.front());
      conn.video = video_encoder.back();
    }

    const std::string vcodec = conf->GetVideoEncoder();
    if (elements::encoders::IsH264Encoder(vcodec)) {
      elements::parser::ElementH264Parse *premux_parser =
          elements::parser::make_h264_parser(0);
      ElementAdd(premux_parser);
      ElementLink(conn.video, premux_parser);
      conn.video = premux_parser;
    }

    elements::ElementTee *tee =
        new elements::ElementTee(common::MemSPrintf(VIDEO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.video, tee);
    conn.video = tee;
  }

  if (conf->HaveAudio()) {
    elements_line_t audio_encoder_line = BuildAudioConverter(0);
    if (!audio_encoder_line.empty()) {
      ElementLink(conn.audio, audio_encoder_line.front());
      conn.audio = audio_encoder_line.back();
    }

    const std::string acodec = conf->GetAudioEncoder();
    if (elements::encoders::IsAACEncoder(acodec)) {
      elements::parser::ElementAACParse *premux_parser =
          elements::parser::make_aac_parser(0);
      ElementAdd(premux_parser);
      ElementLink(conn.audio, premux_parser);
      conn.audio = premux_parser;
    }

    elements::ElementTee *tee =
        new elements::ElementTee(common::MemSPrintf(AUDIO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.audio, tee);
    conn.audio = tee;
  }
  return conn;
}

elements_line_t
EncodingStreamBuilder::BuildVideoPostProc(element_id_t video_id) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);
  elements::Element *first = nullptr;
  elements::Element *last = nullptr;

  const int width = conf->GetWidth();
  const int height = conf->GetHeight();
  const int framerate = conf->GetFramerate();
  if (conf->IsGpu()) {
    if (conf->IsMfxGpu()) {
      elements::ElementMFXVpp *post = new elements::ElementMFXVpp(
          common::MemSPrintf(POST_PROC_NAME_1U, video_id));
      post->SetForceAspectRatio(false);
      if (width != DEFAULT_VIDEO_WIDTH && height != DEFAULT_VIDEO_HEIGHT) {
        post->SetWidth(width);
        post->SetHeight(height);
      }
      if (framerate != DEFAULT_FRAME_RATE) {
        post->SetFrameRate(framerate);
      }
      if (conf->GetDeinterlace()) {
        post->SetDinterlaceMode(1);
      }
      first = post;
    } else {
      elements::ElementVaapiPostProc *post = new elements::ElementVaapiPostProc(
          common::MemSPrintf(POST_PROC_NAME_1U, video_id));
      if (!conf->GetDeinterlace()) {
        post->SetDinterlaceMode(2); // (2): disabled - Never deinterlace
      }
      post->SetFormat(2); // GST_VIDEO_FORMAT_I420
      post->SetForceAspectRatio(false);
      first = post;
    }

    ElementAdd(first);
  } else {
    elements_line_t first_last = elements::encoders::build_video_convert(
        conf->GetDeinterlace(), this, video_id);
    first = first_last.front();
    last = first_last.back();

    if (width != DEFAULT_VIDEO_WIDTH && height != DEFAULT_VIDEO_HEIGHT) {
      last = elements::encoders::build_video_scale(width, height, this, last,
                                                   video_id);
    }

    common::media::Rational rat = conf->GetAspectRatio();
    if (rat != unknown_aspect_ratio) {
      elements::video::ElementAspectRatio *aspect_ratio =
          new elements::video::ElementAspectRatio(
              common::MemSPrintf(ASPECT_RATIO_NAME_1U, video_id));
      aspect_ratio->SetAspectRatio(rat.num, rat.den);
      ElementAdd(aspect_ratio);
      ElementLink(last, aspect_ratio);
      last = aspect_ratio;
    }

    if (framerate != DEFAULT_FRAME_RATE) {
      last = elements::encoders::build_video_framerate(framerate, this, last,
                                                       video_id);
    }
  }

  common::uri::Url logo_uri = conf->GetLogoPath();
  common::draw::Point logo_point = conf->GetLogoPos();
  alpha_t alpha = conf->GetLogoAlpha();
  if (logo_uri.IsValid()) {
    elements::video::ElementGDKPixBufOverlay *videologo =
        new elements::video::ElementGDKPixBufOverlay(
            common::MemSPrintf(VIDEO_LOGO_NAME_1U, video_id));
    common::uri::Url::scheme scheme = logo_uri.GetScheme();
    if (scheme == common::uri::Url::file) {
      common::uri::Upath upath = logo_uri.GetPath();
      std::string path = upath.GetPath();
      videologo->SetLocation(path);
    } else {
      NOTREACHED();
    }
    videologo->SetOffsetX(logo_point.x);
    videologo->SetOffsetY(logo_point.y);
    videologo->SetAlpha(alpha);
    ElementAdd(videologo);
    ElementLink(last, videologo);
    last = videologo;
  }

  return {first, last};
}

elements_line_t
EncodingStreamBuilder::BuildAudioPostProc(element_id_t audio_id) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);

  const volume_t volume = conf->GetVolume();
  audio_channel_count_t achannels = conf->GetAudioChannels();
  elements_line_t first_last = elements::encoders::build_audio_converters(
      volume, achannels, this, audio_id);
  return first_last;
}

elements_line_t
EncodingStreamBuilder::BuildVideoConverter(element_id_t video_id) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);

  int video_bitrate = conf->GetVideoBitrate();
  elements_line_t video_encoder = elements::encoders::build_video_encoder(
      conf->GetVideoEncoder(), video_bitrate, conf->GetVideoEncoderArgs(),
      conf->GetVideoEncoderStrArgs(), this, video_id);
  return video_encoder;
}

elements_line_t
EncodingStreamBuilder::BuildAudioConverter(element_id_t audio_id) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);

  const std::string audio_encoder_str = conf->GetAudioEncoder();
  int audiorate = conf->GetAudioBitrate();
  std::string name_codec = common::MemSPrintf(AUDIO_CODEC_NAME_1U, audio_id);
  elements::Element *enc = elements::encoders::make_audio_encoder(
      audio_encoder_str, name_codec, audiorate);
  ElementAdd(enc);
  elements::Element *first = enc;
  elements::Element *last = enc;
  if (audio_encoder_str == elements::encoders::ElementMP3Enc::GetPluginName()) {
    elements::audio::ElementAudioResample *audioresample =
        new elements::audio::ElementAudioResample(
            common::MemSPrintf(AUDIO_RESAMPLE_NAME_1U, audio_id));
    elements::parser::ElementMPEGAudioParse *mpegaudioparse =
        new elements::parser::ElementMPEGAudioParse(
            common::MemSPrintf(MPEG_AUDIO_PARSE_NAME_1U, audio_id));

    ElementAdd(audioresample);
    ElementAdd(mpegaudioparse);

    first = audioresample;
    ElementLink(audioresample, enc);
    ElementLink(enc, mpegaudioparse);
    last = mpegaudioparse;
  }

  return {first, last};
}

} // namespace builders
} // namespace streams
} // namespace stream
} // namespace iptv_cloud
