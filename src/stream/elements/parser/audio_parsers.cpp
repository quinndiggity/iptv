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

#include "stream/elements/parser/audio_parsers.h"

#include "gst_constants.h"  // for AAC_PARSE, AC3_PARSE, MPEG_AUDIO_PARSE

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace parser {

ElementAACParse* make_aac_parser(element_id_t parser_id) {
  return make_audio_parser<ElementAACParse>(parser_id);
}

ElementMPEGAudioParse* make_mpeg_parser(element_id_t parser_id) {
  return make_audio_parser<ElementMPEGAudioParse>(parser_id);
}

ElementAC3Parse* make_ac3_parser(element_id_t parser_id) {
  return make_audio_parser<ElementAC3Parse>(parser_id);
}

Element* make_audio_parser(const std::string& parser, const std::string& name) {
  if (parser == ElementAACParse::GetPluginName()) {
    return new ElementAACParse(name);
  } else if (parser == ElementAC3Parse::GetPluginName()) {
    return new ElementAC3Parse(name);
  } else if (parser == ElementMPEGAudioParse::GetPluginName()) {
    return new ElementMPEGAudioParse(name);
  }

  NOTREACHED() << "Please register new audio parser type: " << parser;
  return nullptr;
}

}  // namespace parser
}  // namespace elements
}  // namespace stream
}  // namespace iptv_cloud
