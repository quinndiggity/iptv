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

#include "stream/streams/src_decodebin_stream.h"

#include "stream/pad/pad.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

void SrcDecodeBinStream::ConnectDecodebinSignals(
    elements::ElementDecodebin *decodebin) {
  gboolean pad_added =
      decodebin->RegisterPadAddedCallback(decodebin_pad_added_callback, this);
  DCHECK(pad_added);

  gboolean autoplug_continue =
      decodebin->RegisterAutoplugContinue(decodebin_autoplugger_callback, this);
  DCHECK(autoplug_continue);

  gboolean autoplug_select = decodebin->RegisterAutoplugSelect(
      decodebin_autoplug_select_callback, this);
  DCHECK(autoplug_select);

  gboolean autoplug_sort =
      decodebin->RegisterAutoplugSort(decodebin_autoplug_sort_callback, this);
  DCHECK(autoplug_sort);

  gboolean element_added =
      decodebin->RegisterElementAdded(decodebin_element_added_callback, this);
  DCHECK(element_added);

  gboolean element_removed = decodebin->RegisterElementRemoved(
      decodebin_element_removed_callback, this);
  DCHECK(element_removed);
}

SrcDecodeBinStream::SrcDecodeBinStream(Config *config, IStreamClient *client,
                                       StreamStruct *stats)
    : IBaseStream(config, client, stats) {}

SrcDecodeBinStream::~SrcDecodeBinStream() {}

const char *SrcDecodeBinStream::ClassName() const {
  return "SrcDecodeBinStream";
}

void SrcDecodeBinStream::PreLoop() {
  Config *conf = GetApi();
  input_t input = conf->GetInput();
  if (client_) {
    client_->OnInputChanged(input[0]);
  }
}

void SrcDecodeBinStream::PostLoop(ExitStatus status) { UNUSED(status); }

void SrcDecodeBinStream::decodebin_pad_added_callback(GstElement *src,
                                                      GstPad *new_pad,
                                                      gpointer user_data) {
  SrcDecodeBinStream *stream =
      reinterpret_cast<SrcDecodeBinStream *>(user_data);
  stream->HandleDecodeBinPadAdded(src, new_pad);
}

gboolean SrcDecodeBinStream::decodebin_autoplugger_callback(
    GstElement *elem, GstPad *pad, GstCaps *caps, gpointer user_data) {
  SrcDecodeBinStream *stream =
      reinterpret_cast<SrcDecodeBinStream *>(user_data);
  return stream->HandleDecodeBinAutoplugger(elem, pad, caps);
}

GstAutoplugSelectResult SrcDecodeBinStream::decodebin_autoplug_select_callback(
    GstElement *bin, GstPad *pad, GstCaps *caps, GstElementFactory *factory,
    gpointer user_data) {
  SrcDecodeBinStream *stream =
      reinterpret_cast<SrcDecodeBinStream *>(user_data);
  return stream->HandleAutoplugSelect(bin, pad, caps, factory);
}

GValueArray *SrcDecodeBinStream::decodebin_autoplug_sort_callback(
    GstElement *bin, GstPad *pad, GstCaps *caps, GValueArray *factories,
    gpointer user_data) {
  SrcDecodeBinStream *stream =
      reinterpret_cast<SrcDecodeBinStream *>(user_data);
  return stream->HandleAutoplugSort(bin, pad, caps, factories);
}

void SrcDecodeBinStream::decodebin_element_added_callback(GstBin *bin,
                                                          GstElement *element,
                                                          gpointer user_data) {
  SrcDecodeBinStream *stream =
      reinterpret_cast<SrcDecodeBinStream *>(user_data);
  return stream->HandleDecodeBinElementAdded(bin, element);
}

void SrcDecodeBinStream::decodebin_element_removed_callback(
    GstBin *bin, GstElement *element, gpointer user_data) {
  SrcDecodeBinStream *stream =
      reinterpret_cast<SrcDecodeBinStream *>(user_data);
  return stream->HandleDecodeBinElementRemoved(bin, element);
}

void SrcDecodeBinStream::OnInpudSrcPadCreated(common::uri::Url::scheme scheme,
                                              pad::Pad *src_pad,
                                              element_id_t id) {
  UNUSED(scheme);
  LinkInputPad(src_pad->GetGstPad(), id);
}

void SrcDecodeBinStream::OnOutputSinkPadCreated(common::uri::Url::scheme scheme,
                                                pad::Pad *sink_pad,
                                                element_id_t id) {
  UNUSED(scheme);
  LinkOutputPad(sink_pad->GetGstPad(), id);
}

void SrcDecodeBinStream::OnDecodebinCreated(
    elements::ElementDecodebin *decodebin) {
  ConnectDecodebinSignals(decodebin);
}

} // namespace streams
} // namespace stream
} // namespace iptv_cloud
