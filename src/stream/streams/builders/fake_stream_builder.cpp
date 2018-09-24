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

#include "stream/streams/builders/fake_stream_builder.h"

#include "stream/pad/pad.h" // for Pads

#include "stream/elements/sink/fake.h" // for make_fake_sink, ElementFakeSink (ptr...

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

FakeStreamBuilder::FakeStreamBuilder(EncodingConfig *api,
                                     SrcDecodeBinStream *observer)
    : EncodingStreamBuilder(api, observer) {}

Connector FakeStreamBuilder::BuildOutput(Connector conn) {
  return BuildFakeOutput(conn);
}

Connector FakeStreamBuilder::BuildFakeOutput(Connector conn) {
  EncodingConfig *conf = static_cast<EncodingConfig *>(api_);
  if (conf->HaveVideo()) {
    elements::sink::ElementFakeSink *video = elements::sink::make_fake_sink(0);
    ElementAdd(video);
    pad::Pad *sink_pad = video->StaticPad("sink");
    if (sink_pad->IsValid()) {
      HandleOutputSinkPadCreated(common::uri::Url::unknown, sink_pad, 0);
    }
    delete sink_pad;
    if (conn.video) {
      ElementLink(conn.video, video);
    }
  }

  if (conf->HaveAudio()) {
    elements::sink::ElementFakeSink *audio = elements::sink::make_fake_sink(1);
    ElementAdd(audio);
    pad::Pad *sink_pad = audio->StaticPad("sink");
    if (sink_pad->IsValid()) {
      HandleOutputSinkPadCreated(common::uri::Url::unknown, sink_pad, 0);
    }
    delete sink_pad;
    if (conn.audio) {
      ElementLink(conn.audio, audio);
    }
  }

  return conn;
}

} // namespace builders
} // namespace streams
} // namespace stream
} // namespace iptv_cloud
