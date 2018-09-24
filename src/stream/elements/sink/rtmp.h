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

// for element_id_t

#include "stream/elements/element.h" // for SupportedElements::ELEMENT_RTMP_SINK
#include "stream/elements/sink/sink.h" // for ElementSync

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sink {

class ElementRtmpSink : public ElementSync<ELEMENT_RTMP_SINK> {
public:
  typedef ElementSync<ELEMENT_RTMP_SINK> base_class;
  using base_class::base_class;

  void SetLocation(const std::string &location);
};

ElementRtmpSink *make_rtmp_sink(element_id_t sink_id,
                                const std::string &output);

} // namespace sink
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
