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

#include "stream/streams/fake_stream.h"

#include "stream/streams/builders/fake_stream_builder.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

FakeStream::FakeStream(EncodingConfig* config, IStreamClient* client)
    : EncodingStream(config, client, new StreamStruct(StreamInfo{"fake", ENCODING, {0}, {1}})) {}

const char* FakeStream::ClassName() const {
  return "FakeStream";
}

FakeStream::~FakeStream() {
  StreamStruct* stat = GetStats();
  delete stat;
}

gboolean FakeStream::HandleMainTimerTick() {
  return TRUE;
}

void FakeStream::PreLoop() {}

void FakeStream::PostLoop(ExitStatus status) {
  UNUSED(status);
}

IBaseBuilder* FakeStream::CreateBuilder() {
  EncodingConfig* econf = static_cast<EncodingConfig*>(GetApi());
  return new builders::FakeStreamBuilder(econf, this);
}

gboolean FakeStream::HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  GstMessageType type = GST_MESSAGE_TYPE(message);
  if (type == GST_MESSAGE_ERROR) {
    GError* err = NULL;
    gchar* err_msg = NULL;

    gst_message_parse_error(message, &err, &err_msg);
    if (err->code == G_FILE_ERROR_NOENT) {
      Quit(EXIT_INNER);
    }
    g_error_free(err);
    g_free(err_msg);
  }
  return EncodingStream::HandleAsyncBusMessageReceived(bus, message);
}

void FakeStream::HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  UNUSED(element);
}

void FakeStream::HandleDecodeBinElementRemoved(GstBin* bin, GstElement* element) {
  UNUSED(bin);
  UNUSED(element);
}

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
