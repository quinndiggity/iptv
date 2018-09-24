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

#include "stream/streams/screen_stream.h"

#include "stream/streams/builders/screen_stream_builder.h"

#include "stream/pad/pad.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

ScreenStream::ScreenStream(AudioVideoConfig *config, IStreamClient *client,
                           StreamStruct *stats)
    : IBaseStream(config, client, stats) {}

const char *ScreenStream::ClassName() const { return "ScreenStream"; }

void ScreenStream::OnInpudSrcPadCreated(common::uri::Url::scheme scheme,
                                        pad::Pad *src_pad, element_id_t id) {
  UNUSED(scheme);
  UNUSED(src_pad);
  UNUSED(id);
  // LinkInputPad(sink_pad);
}

void ScreenStream::OnOutputSinkPadCreated(common::uri::Url::scheme scheme,
                                          pad::Pad *sink_pad, element_id_t id) {
  UNUSED(scheme);
  LinkOutputPad(sink_pad->GetGstPad(), id);
}

IBaseBuilder *ScreenStream::CreateBuilder() {
  AudioVideoConfig *aconf = static_cast<AudioVideoConfig *>(GetApi());
  return new builders::ScreenStreamBuilder(aconf, this);
}

void ScreenStream::PreLoop() {}

void ScreenStream::PostLoop(ExitStatus status) { UNUSED(status); }

} // namespace streams
} // namespace stream
} // namespace iptv_cloud
