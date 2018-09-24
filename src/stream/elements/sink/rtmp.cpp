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

#include "stream/elements/sink/rtmp.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sink {

void ElementRtmpSink::SetLocation(const std::string &location) {
  SetProperty("location", location);
}

ElementRtmpSink *make_rtmp_sink(element_id_t sink_id,
                                const std::string &output) {
  ElementRtmpSink *rtmp_out = make_sink<ElementRtmpSink>(sink_id);
  rtmp_out->SetLocation(output);
  return rtmp_out;
}

} // namespace sink
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
