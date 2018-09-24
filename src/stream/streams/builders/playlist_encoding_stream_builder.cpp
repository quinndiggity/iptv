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

#include "stream/streams/builders/playlist_encoding_stream_builder.h"

#include "stream/streams/playlist_encoding_stream.h"

#include "stream/elements/sources/appsrc.h"

#include "stream/pad/pad.h"

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

PlaylistEncodingStreamBuilder::PlaylistEncodingStreamBuilder(
    PlaylistEncodingConfig *api, PlaylistEncodingStream *observer)
    : EncodingStreamBuilder(api, observer) {}

elements::Element *PlaylistEncodingStreamBuilder::BuildInputSrc() {
  elements::sources::ElementAppSrc *appsrc = elements::sources::make_app_src(0);

  // g_signal_connect(appsrc, "enough-data", G_CALLBACK(enough_data), this);
  pad::Pad *src_pad = appsrc->StaticPad("src");
  if (src_pad->IsValid()) {
    HandleInputSrcPadCreated(common::uri::Url::file, src_pad, 0);
  }
  delete src_pad;
  ElementAdd(appsrc);
  HandleAppSrcCreated(appsrc);
  return appsrc;
}

void PlaylistEncodingStreamBuilder::HandleAppSrcCreated(
    elements::sources::ElementAppSrc *src) {
  if (observer_) {
    PlaylistEncodingStream *pobs =
        static_cast<PlaylistEncodingStream *>(observer_);
    pobs->OnAppSrcCreatedCreated(src);
  }
}

} // namespace builders
} // namespace streams
} // namespace stream
} // namespace iptv_cloud
