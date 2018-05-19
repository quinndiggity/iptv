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

#include "stream/streams/builders/relay_stream_builder.h"

#include <common/sprintf.h>

#include "stream/elements/parser/audio_parsers.h"
#include "stream/elements/parser/video_parsers.h"

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

RelayStreamBuilder::RelayStreamBuilder(RelayConfig* config, SrcDecodeBinStream* observer)
    : SrcDecodeStreamBuilder(config, observer) {}

Connector RelayStreamBuilder::BuildPostProc(Connector conn) {
  return conn;
}

elements::Element* RelayStreamBuilder::BuildVideoUdbConnection() {
  RelayConfig* rconfig = static_cast<RelayConfig*>(api_);
  const std::string video_parser = rconfig->GetVideoParser();
  elements::Element* video = elements::parser::make_video_parser(
      video_parser, common::MemSPrintf(UDB_VIDEO_NAME_1U, 0));  // tsparse, h264parse
  return video;
}

elements::Element* RelayStreamBuilder::BuildAudioUdbConnection() {
  RelayConfig* rconfig = static_cast<RelayConfig*>(api_);
  const std::string audio_parser = rconfig->GetAudioParser();
  elements::Element* audio =
      elements::parser::make_audio_parser(audio_parser, common::MemSPrintf(UDB_AUDIO_NAME_1U, 0));
  return audio;
}

SupportedVideoCodecs RelayStreamBuilder::GetVideoCodecType() const {
  RelayConfig* conf = static_cast<RelayConfig*>(api_);
  const std::string vparser = conf->GetVideoParser();
  if (vparser == elements::parser::ElementH264Parse::GetPluginName()) {
    return VIDEO_H264_CODEC;
  } else if (vparser == elements::parser::ElementH265Parse::GetPluginName()) {
    return VIDEO_H265_CODEC;
  } else if (vparser == elements::parser::ElementTsParse::GetPluginName()) {
    return VIDEO_MPEG_CODEC;
  }

  NOTREACHED();
  return VIDEO_H264_CODEC;
}

SupportedAudioCodecs RelayStreamBuilder::GetAudioCodecType() const {
  RelayConfig* conf = static_cast<RelayConfig*>(api_);
  const std::string aparser = conf->GetAudioParser();
  if (aparser == elements::parser::ElementAACParse::GetPluginName()) {
    return AUDIO_AAC_CODEC;
  } else if (aparser == elements::parser::ElementMPEGAudioParse::GetPluginName()) {
    return AUDIO_MPEG_CODEC;
  } else if (aparser == elements::parser::ElementAC3Parse::GetPluginName()) {
    return AUDIO_AC3_CODEC;
  }

  NOTREACHED();
  return AUDIO_MPEG_CODEC;
}
Connector RelayStreamBuilder::BuildConverter(Connector conn) {
  RelayConfig* rconfig = static_cast<RelayConfig*>(api_);
  if (rconfig->HaveVideo()) {
    elements::ElementTee* tee = new elements::ElementTee(common::MemSPrintf(VIDEO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.video, tee);
    conn.video = tee;
  }
  if (rconfig->HaveAudio()) {
    elements::ElementTee* tee = new elements::ElementTee(common::MemSPrintf(AUDIO_TEE_NAME_1U, 0));
    ElementAdd(tee);
    ElementLink(conn.audio, tee);
    conn.audio = tee;
  }
  return conn;
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
