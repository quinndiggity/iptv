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

#include <cairo.h>  // for cairo_t
#include <string>   // for string

#include "stream/stypes.h"

#include "stream/elements/element.h"  // for ElementEx, Element (ptr only), GstElement

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace video {

typedef ElementEx<ELEMENT_INTERLACE> ElementInterlace;

typedef ElementEx<ELEMENT_AV_DEINTERLACE> ElementAvDeinterlace;

class ElementGDKPixBufOverlay : public ElementEx<ELEMENT_GDK_PIXBUF_OVERLAY> {
 public:
  typedef ElementEx<ELEMENT_GDK_PIXBUF_OVERLAY> base_class;
  using base_class::base_class;

  void SetOffsetX(gint x = 0);                    // Range: -2147483648 - 2147483647 Default: 0
  void SetOffsetY(gint y = 0);                    // Range: -2147483648 - 2147483647 Default: 0
  void SetLocation(const std::string& location);  // Default: null
  void SetAlpha(alpha_t alpha = 1);               // Range: 0 - 1 Default: 1
};

class ElementDeinterlace : public ElementEx<ELEMENT_DEINTERLACE> {
 public:
  typedef ElementEx<ELEMENT_DEINTERLACE> base_class;
  using base_class::base_class;

  void SetMethod(int method = 4);  // Default: Linear (4)
};

Element* make_video_deinterlace(const std::string& deinterlace, const std::string& name);

typedef ElementEx<ELEMENT_AUTO_VIDEO_CONVERT> ElementAutoVideoConvert;
typedef ElementEx<ELEMENT_VIDEO_CONVERT> ElementVideoConvert;
typedef ElementEx<ELEMENT_VIDEO_SCALE> ElementVideoScale;
typedef ElementEx<ELEMENT_VIDEO_RATE> ElementVideoRate;

class ElementAspectRatio : public ElementEx<ELEMENT_ASPECT_RATIO> {
 public:
  typedef ElementEx<ELEMENT_ASPECT_RATIO> base_class;
  using base_class::base_class;

  void SetAspectRatio(int num, int den);
};

typedef ElementEx<ELEMENT_IMAGE_FREEZE> ElementImageFreeze;
typedef ElementEx<ELEMENT_VIDEO_BOX> ElementVideoBox;
typedef ElementEx<ELEMENT_VIDEO_MIXER> ElementVideoMixer;
typedef ElementEx<ELEMENT_VIDEO_CROP> ElementVideoCrop;

class ElementCairoOverlay : public ElementEx<ELEMENT_CAIRO_OVERLAY> {
 public:
  typedef ElementEx<ELEMENT_CAIRO_OVERLAY> base_class;
  using base_class::base_class;

  typedef void (
      *draw_callback_t)(GstElement* overlay, cairo_t* cr, guint64 timestamp, guint64 duration, gpointer user_data);

  gboolean RegisterDrawCallback(draw_callback_t cb, gpointer user_data) WARN_UNUSED_RESULT;
};

}  // namespace video
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
