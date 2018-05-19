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

#include "stream/elements/sink/build_output.h"

#include <common/uri/url.h>

#include "output_uri.h"  // for OutputUri, IsFakeUrl

#include "stream/elements/sink/http.h"  // for build_http_sink, HlsOutput
#include "stream/elements/sink/rtmp.h"  // for build_rtmp_sink
#include "stream/elements/sink/tcp.h"
#include "stream/elements/sink/udp.h"  // for build_udp_sink

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sink {

Element* build_output(const OutputUri& output, element_id_t sink_id) {
  common::uri::Url uri = output.GetOutput();
  common::uri::Url::scheme scheme = uri.GetScheme();

  if (scheme == common::uri::Url::udp) {
    const std::string url = uri.GetHost();
    common::net::HostAndPort host;
    if (!common::ConvertFromString(url, &host)) {
      NOTREACHED() << "Unknownt output url: " << url;
      return nullptr;
    }
    ElementUDPSink* udp_sink = elements::sink::make_udp_sink(host, sink_id);
    return udp_sink;
  } else if (scheme == common::uri::Url::tcp) {
    const std::string url = uri.GetHost();
    common::net::HostAndPort host;
    if (!common::ConvertFromString(url, &host)) {
      NOTREACHED() << "Unknownt output url: " << url;
      return nullptr;
    }
    ElementTCPServerSink* tcp_sink = elements::sink::make_tcp_server_sink(host, sink_id);
    return tcp_sink;
  } else if (scheme == common::uri::Url::rtmp) {
    ElementRtmpSink* rtmp_sink = elements::sink::make_rtmp_sink(sink_id, uri.GetUrl());
    return rtmp_sink;
  } else if (scheme == common::uri::Url::http) {
    const common::file_system::ascii_directory_string_path http_root = output.GetHttpRoot();
    const common::uri::Upath upath = uri.GetPath();
    const std::string filename = upath.GetFileName();
    elements::sink::HlsOutput hout = MakeHlsOutput(uri, http_root, filename);
    ElementHLSSink* http_sink = elements::sink::make_http_sink(sink_id, hout);
    return http_sink;
  }

  NOTREACHED() << "Unknownt output url: " << uri.GetUrl();
  return nullptr;
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
