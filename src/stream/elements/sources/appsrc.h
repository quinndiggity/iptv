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

#include <gst/gstpad.h>

#include "stream/elements/sources/sources.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

class ElementAppSrc : public ElementEx<ELEMENT_APP_SRC> {
public:
  typedef ElementEx<ELEMENT_APP_SRC> base_class;
  typedef void (*need_data_callback_t)(GstElement *pipeline, guint size,
                                       gpointer user_data);
  using base_class::base_class;

  gboolean RegisterNeedDataCallback(need_data_callback_t cb,
                                    gpointer user_data) WARN_UNUSED_RESULT;

  GstFlowReturn PushBuffer(GstBuffer *buffer);
  void SendEOS();
};

ElementAppSrc *make_app_src(element_id_t input_id);

} // namespace sources
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
