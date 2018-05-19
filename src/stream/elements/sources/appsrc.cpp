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

#include "stream/elements/sources/appsrc.h"

#include <gst/app/gstappsrc.h>  // for GST_APP_SRC

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

gboolean ElementAppSrc::RegisterNeedDataCallback(need_data_callback_t cb, gpointer user_data) {
  return RegisterCallback("need-data", G_CALLBACK(cb), user_data);
}

GstFlowReturn ElementAppSrc::PushBuffer(GstBuffer* buffer) {
  return gst_app_src_push_buffer(GST_APP_SRC(GetGstElement()), buffer);
}

void ElementAppSrc::SendEOS() {
  gst_app_src_end_of_stream(GST_APP_SRC(GetGstElement()));  // send  eos
}

ElementAppSrc* make_app_src(element_id_t input_id) {
  return make_sources<ElementAppSrc>(input_id);
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
