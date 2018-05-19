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

#include "stream/elements/sources/udpsrc.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

void ElementUDPSrc::SetUri(const std::string& uri) {
  SetProperty("uri", uri);
}

void ElementUDPSrc::SetAddress(const std::string& host) {
  SetProperty("address", host);
}

void ElementUDPSrc::SetPort(uint16_t port) {
  SetProperty("port", port);
}

ElementUDPSrc* make_udp_src(const common::net::HostAndPort& host, element_id_t input_id) {
  ElementUDPSrc* udpsrc = make_sources<ElementUDPSrc>(input_id);
  udpsrc->SetAddress(host.GetHost());
  udpsrc->SetPort(host.GetPort());
  return udpsrc;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
