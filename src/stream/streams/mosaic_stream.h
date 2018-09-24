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

#include <cairo.h>

#include <gst/gst.h>

#include "stream/ibase_stream.h"
#include "stream/streams/configs/encoding_config.h"

#include "stream/streams/mosaic_options.h"

namespace iptv_cloud {
namespace stream {

namespace elements {
class Element;
}

namespace elements {
class ElementDecodebin;
}

namespace elements {
namespace video {
class ElementCairoOverlay;
}
} // namespace elements

namespace streams {

namespace builders {
class MosaicStreamBuilder;
}

class MosaicStream : public IBaseStream {
  friend class builders::MosaicStreamBuilder;

public:
  MosaicStream(EncodingConfig *config, IStreamClient *client,
               StreamStruct *stats);
  virtual const char *ClassName() const override;

protected:
  virtual void OnInpudSrcPadCreated(common::uri::Url::scheme scheme,
                                    pad::Pad *src_pad,
                                    element_id_t id) override;
  virtual void OnOutputSinkPadCreated(common::uri::Url::scheme scheme,
                                      pad::Pad *sink_pad,
                                      element_id_t id) override;

  virtual void OnDecodebinCreated(elements::ElementDecodebin *decodebin);
  virtual void OnCairoCreated(elements::video::ElementCairoOverlay *cairo,
                              const MosaicImageOptions &options);

  virtual IBaseBuilder *CreateBuilder() override;

  virtual void PreLoop() override;
  virtual void PostLoop(ExitStatus status) override;

  virtual void ConnectDecodebinSignals(elements::ElementDecodebin *decodebin);
  virtual void ConnectCairoSignals(elements::video::ElementCairoOverlay *cairo,
                                   const MosaicImageOptions &options);

  virtual gboolean HandleAsyncBusMessageReceived(GstBus *bus,
                                                 GstMessage *message) override;
  virtual gboolean HandleDecodeBinAutoplugger(GstElement *elem, GstPad *pad,
                                              GstCaps *caps);
  virtual void HandleDecodeBinPadAdded(GstElement *src, GstPad *new_pad);
  virtual GValueArray *HandleAutoplugSort(GstElement *bin, GstPad *pad,
                                          GstCaps *caps,
                                          GValueArray *factories);
  virtual void HandleElementAdded(GstBin *bin, GstElement *element);

  virtual void HandleCairoDraw(GstElement *overlay, cairo_t *cr,
                               guint64 timestamp, guint64 duration);

private:
  static void decodebin_pad_added_callback(GstElement *src, GstPad *new_pad,
                                           gpointer user_data);
  static gboolean decodebin_autoplugger_callback(GstElement *elem, GstPad *pad,
                                                 GstCaps *caps,
                                                 gpointer user_data);
  static GValueArray *
  decodebin_autoplug_sort_callback(GstElement *bin, GstPad *pad, GstCaps *caps,
                                   GValueArray *factories, gpointer user_data);
  static void decodebin_element_added_callback(GstBin *bin, GstElement *element,
                                               gpointer user_data);

  static void cairo_draw_callback(GstElement *overlay, cairo_t *cr,
                                  guint64 timestamp, guint64 duration,
                                  gpointer user_data);

  MosaicImageOptions options_;
};

} // namespace streams
} // namespace stream
} // namespace iptv_cloud
