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

#include "stream/stypes.h"

#include "stream/elements/element.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace parser {

class ElementAACParse : public ElementEx<ELEMENT_AAC_PARSE> {
 public:
  typedef ElementEx<ELEMENT_AAC_PARSE> base_class;
  using base_class::base_class;
};

class ElementAC3Parse : public ElementEx<ELEMENT_AC3_PARSE> {
 public:
  typedef ElementEx<ELEMENT_AC3_PARSE> base_class;
  using base_class::base_class;
};

class ElementMPEGAudioParse : public ElementEx<ELEMENT_MPEG_AUDIO_PARSE> {
 public:
  typedef ElementEx<ELEMENT_MPEG_AUDIO_PARSE> base_class;
  using base_class::base_class;
};

template <typename T>
T* make_audio_parser(element_id_t parser_id) {
  return make_element<T>(common::MemSPrintf(AUDIO_PARSER_NAME_1U, parser_id));
}

ElementAACParse* make_aac_parser(element_id_t parser_id);
ElementAC3Parse* make_ac3_parser(element_id_t parser_id);
ElementMPEGAudioParse* make_mpeg_parser(element_id_t parser_id);

Element* make_audio_parser(const std::string& parser, const std::string& name);

}  // namespace parser
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
