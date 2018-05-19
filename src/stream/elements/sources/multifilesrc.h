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

#include "stream/elements/element.h"          // for SupportedElements::ELEMENT_MUL...
#include "stream/elements/sources/sources.h"  // for ElementLocation

// for element_id_t

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

struct MultiFileSrcInfo {
  std::string location;
  int index;
  bool loop;
};

class ElementMultiFileSrc : public ElementLocation<ELEMENT_MULTIFILE_SRC> {
 public:
  typedef ElementLocation<ELEMENT_MULTIFILE_SRC> base_class;
  using base_class::base_class;

  void SetIndex(int index = 0);     // Range: 0 - 2147483647 Default: 0
  void SetLoop(bool loop = false);  // Default: false
};

ElementMultiFileSrc* make_multifile_src(const MultiFileSrcInfo& info, element_id_t input_id);

}  // namespace sources
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
