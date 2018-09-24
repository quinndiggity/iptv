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

#include "stream/elements/sources/build_input.h"

#include <stddef.h> // for NULL

#include "stream/elements/sources/filesrc.h"
#include "stream/elements/sources/httpsrc.h"
#include "stream/elements/sources/rtmpsrc.h"
#include "stream/elements/sources/tcpsrc.h"
#include "stream/elements/sources/udpsrc.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

Element *make_src(const common::uri::Url &uri, element_id_t input_id,
                  gint timeout_secs) {
  common::uri::Url::scheme scheme = uri.GetScheme();
  if (scheme == common::uri::Url::file) {
    const common::uri::Upath upath = uri.GetPath();
    return make_file_src(upath.GetPath(), input_id);
  } else if (scheme == common::uri::Url::http ||
             scheme == common::uri::Url::https) {
    return make_http_src(uri.GetUrl(), timeout_secs, input_id);
  } else if (scheme == common::uri::Url::udp) {
    // udp://localhost:8080
    std::string host_str = uri.GetHost();
    common::net::HostAndPort host;
    if (!common::ConvertFromString(host_str, &host)) {
      NOTREACHED() << "Unknownt input url: " << host_str;
      return nullptr;
    }
    return make_udp_src(host, input_id);
  } else if (scheme == common::uri::Url::rtmp) {
    return make_rtmp_src(uri.GetUrl(), timeout_secs, input_id);
  } else if (scheme == common::uri::Url::tcp) {
    // tcp://localhost:8080
    std::string host_str = uri.GetHost();
    common::net::HostAndPort host;
    if (!common::ConvertFromString(host_str, &host)) {
      NOTREACHED() << "Unknownt input url: " << host_str;
      return nullptr;
    }
    return make_tcp_server_src(host, input_id);
  }

  NOTREACHED() << "Unknownt input url: " << uri.GetUrl();
  return nullptr;
}

} // namespace sources
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
