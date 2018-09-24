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

#include "stream/elements/element.h" // for SupportedElements::ELEMENT_FIL...
#include "stream/elements/sources/sources.h" // for ElementLocation

// for element_id_t

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {

class ElementFileSrc : public ElementLocation<ELEMENT_FILE_SRC> {
public:
  typedef ElementLocation<ELEMENT_FILE_SRC> base_class;
  using base_class::base_class;
};

ElementFileSrc *make_file_src(const std::string &location,
                              element_id_t input_id);

} // namespace sources
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
