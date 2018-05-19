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

#include "stream/streams/playlist_encoding_stream.h"

#include <gst/app/gstappsrc.h>  // for GST_APP_SRC

#include "stream/elements/sources/appsrc.h"

#include "stream/streams/builders/playlist_encoding_stream_builder.h"

#define BUFFER_SIZE 4096

namespace iptv_cloud {
namespace stream {
namespace streams {

PlaylistEncodingStream::PlaylistEncodingStream(EncodingConfig* config, IStreamClient* client, StreamStruct* stats)
    : EncodingStream(config, client, stats), app_src_(nullptr), current_file_(NULL), curent_pos_(0) {}

PlaylistEncodingStream::~PlaylistEncodingStream() {
  if (current_file_) {
    fclose(current_file_);
    current_file_ = NULL;
  }
}

const char* PlaylistEncodingStream::ClassName() const {
  return "PlaylistEncodingStream";
}

void PlaylistEncodingStream::OnAppSrcCreatedCreated(elements::sources::ElementAppSrc* src) {
  app_src_ = src;
  gboolean res = src->RegisterNeedDataCallback(PlaylistEncodingStream::need_data_callback, this);
  DCHECK(res);
}

IBaseBuilder* PlaylistEncodingStream::CreateBuilder() {
  PlaylistEncodingConfig* econf = static_cast<PlaylistEncodingConfig*>(GetApi());
  return new builders::PlaylistEncodingStreamBuilder(econf, this);
}

void PlaylistEncodingStream::PreLoop() {}

void PlaylistEncodingStream::HandleNeedData(GstElement* pipeline, guint rsize) {
  UNUSED(pipeline);
  UNUSED(rsize);

  size_t size = 0;
  char* ptr = NULL;
  while (size == 0) {
    if (!current_file_) {
      current_file_ = OpenNextFile();
    }

    if (!current_file_) {
      app_src_->SendEOS();
      return;
    }

    ptr = static_cast<char*>(calloc(BUFFER_SIZE, sizeof(char)));
    size = fread(ptr, sizeof(char), BUFFER_SIZE, current_file_);
    if (size == 0) {
      fclose(current_file_);
      current_file_ = NULL;
      free(ptr);
    }
  }

  GstBuffer* buffer = gst_buffer_new_wrapped(ptr, size);
  GstFlowReturn ret = app_src_->PushBuffer(buffer);
  if (ret != GST_FLOW_OK) {
    WARNING_LOG() << GetID() << " gst_app_src_push_buffer failed: " << gst_flow_get_name(ret);
    Quit(EXIT_INNER);
  }
}

void PlaylistEncodingStream::need_data_callback(GstElement* pipeline, guint size, gpointer user_data) {
  PlaylistEncodingStream* stream = reinterpret_cast<PlaylistEncodingStream*>(user_data);
  return stream->HandleNeedData(pipeline, size);
}

FILE* PlaylistEncodingStream::OpenNextFile() {
  PlaylistEncodingConfig* econf = static_cast<PlaylistEncodingConfig*>(GetApi());
  if (!econf->GetLoop()) {
    input_t input = econf->GetInput();
    if (curent_pos_ >= input.size()) {
      INFO_LOG() << GetID() << " No more files for playing";
      return NULL;  // EOS
    }
  }

  input_t input = econf->GetInput();
  if (curent_pos_ >= input.size()) {
    curent_pos_ = 0;
  }

  InputUri iuri = input[curent_pos_];
  common::uri::Url uri = iuri.GetInput();
  curent_pos_++;
  common::uri::Upath path = uri.GetPath();
  std::string cur_path = path.GetPath();
  FILE* file = fopen(cur_path.c_str(), "rb");
  if (file) {
    INFO_LOG() << GetID() << " File " << cur_path << " open for playing";
    if (client_) {
      client_->OnInputChanged(iuri);
    }
  } else {
    WARNING_LOG() << GetID() << " File " << cur_path << " can't open for playing";
  }
  return file;
}

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
