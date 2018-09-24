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

#include "stream/streams/builders/test_stream_builder.h"

#include "stream/elements/element.h" // for Element
#include "stream/elements/sources/sources.h"

#include "stream/pad/pad.h" //for Pad

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

TestStreamBuilder::TestStreamBuilder(EncodingConfig *api,
                                     SrcDecodeBinStream *observer)
    : EncodingStreamBuilder(api, observer) {}

Connector TestStreamBuilder::BuildInput() {
  elements::Element *video = nullptr;
  EncodingConfig *econf = static_cast<EncodingConfig *>(api_);
  if (econf->HaveVideo()) {
    video = new elements::sources::ElementVideoTestSrc("video_sdrc");
    ElementAdd(video);
    pad::Pad *src_pad = video->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(common::uri::Url::unknown, src_pad, 0);
    }
    delete src_pad;
  }

  elements::Element *audio = nullptr;
  if (econf->HaveAudio()) {
    audio = new elements::sources::ElementAudioTestSrc("audio_src");
    ElementAdd(audio);
    pad::Pad *src_pad = audio->StaticPad("src");
    if (src_pad->IsValid()) {
      HandleInputSrcPadCreated(common::uri::Url::unknown, src_pad, 0);
    }
    delete src_pad;
  }
  return {video, audio};
}

} // namespace builders
} // namespace streams
} // namespace stream
} // namespace iptv_cloud
