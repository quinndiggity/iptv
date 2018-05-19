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

#include <common/sprintf.h>

#include "stream/elements/element.h"  // for ElementEx, SupportedElements, Supporte...

#include "stream/stypes.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

typedef ElementEx<ELEMENT_VIDEO_TEST_SRC> ElementVideoTestSrc;
typedef ElementEx<ELEMENT_AUDIO_TEST_SRC> ElementAudioTestSrc;

template <SupportedElements el>
class ElementLocation : public ElementEx<el> {
 public:
  typedef ElementEx<el> base_class;
  using base_class::base_class;

  void SetLocation(const std::string& location) { base_class::SetProperty("location", location); }
};

template <typename T>
T* make_sources(element_id_t source_id) {
  return make_element<T>(common::MemSPrintf(SRC_NAME_1U, source_id));
}

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
