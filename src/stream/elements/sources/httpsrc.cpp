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

#include "stream/elements/sources/httpsrc.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

void ElementSoupHTTPSrc::SetIsLive(bool live) {
  SetProperty("is-live", live);
}

void ElementSoupHTTPSrc::SetTimeOut(gint secs) {
  SetProperty("timeout", secs);
}

ElementSoupHTTPSrc* make_http_src(const std::string& location, gint timeout_secs, element_id_t input_id) {
  ElementSoupHTTPSrc* http_src = make_sources<ElementSoupHTTPSrc>(input_id);
  http_src->SetLocation(location);
  http_src->SetTimeOut(timeout_secs);
  return http_src;
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
