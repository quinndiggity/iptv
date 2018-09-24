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

#include <common/net/types.h>

// for element_id_t

#include "stream/elements/element.h" // for ElementEx, SupportedElements::ELEMENT_...
#include "stream/elements/sources/sources.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

class ElementTCPSrc : public ElementEx<ELEMENT_TCP_SERVER_SRC> {
public:
  typedef ElementEx<ELEMENT_TCP_SERVER_SRC> base_class;
  using base_class::base_class;

  void SetHost(
      const std::string &host = "localhost"); // String. Default: "localhost"
  void SetPort(uint16_t port);
};

ElementTCPSrc *make_tcp_server_src(const common::net::HostAndPort &location,
                                   element_id_t input_id);

} // namespace sources
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
