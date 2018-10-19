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

#include "stream/elements/muxer/muxer.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace muxer {

ElementFLVMux* make_flvmux(bool streamable, element_id_t muxer_id) {
  ElementFLVMux* flvmux = make_muxer<ElementFLVMux>(muxer_id);
  flvmux->SetStreamable(streamable);
  return flvmux;
}

ElementMPEGTSMux* make_mpegtsmux(element_id_t muxer_id) {
  return make_muxer<ElementMPEGTSMux>(muxer_id);
}

ElementRTPMux* make_rtpmux(element_id_t muxer_id) {
  return make_muxer<ElementRTPMux>(muxer_id);
}

Element* make_muxer(common::uri::Url::scheme scheme, element_id_t muxer_id) {
  if (scheme == common::uri::Url::rtmp) {
    return make_flvmux(true, muxer_id);
  } else if (scheme == common::uri::Url::udp) {
    return make_rtpmux(muxer_id);
  } else if (scheme == common::uri::Url::tcp) {
    return make_mpegtsmux(muxer_id);
  } else if (scheme == common::uri::Url::http) {
    return make_mpegtsmux(muxer_id);
  }

  NOTREACHED() << "Unknown output scheme: " << scheme;
  return nullptr;
}

void ElementFLVMux::SetStreamable(bool streamable) {
  SetProperty("streamable", streamable);
}

}  // namespace muxer
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
