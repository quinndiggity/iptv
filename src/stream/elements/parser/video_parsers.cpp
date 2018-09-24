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

#include "stream/elements/parser/video_parsers.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace parser {

void ElementH264Parse::SetConfigInterval(gint interval) {
  SetProperty("config-interval", interval);
}

void ElementH264Parse::SetDisablePassthrough(gboolean pass) {
  SetProperty("disable-passthrough", pass);
}

void ElementH265Parse::SetConfigInterval(guint interval) {
  SetProperty("config-interval", interval);
}

ElementMpegParse *make_mpeg2_parser(element_id_t parser_id) {
  return make_video_parser<ElementMpegParse>(parser_id);
}

ElementTsParse *make_ts_parser(element_id_t parser_id) {
  return make_video_parser<ElementTsParse>(parser_id);
}

ElementH264Parse *make_h264_parser(element_id_t parser_id) {
  ElementH264Parse *par = make_video_parser<ElementH264Parse>(parser_id);
  return par;
}

ElementH265Parse *make_h265_parser(element_id_t parser_id) {
  return make_video_parser<ElementH265Parse>(parser_id);
}

Element *make_video_parser(const std::string &parser, const std::string &name) {
  if (parser == ElementH264Parse::GetPluginName()) {
    return new ElementH264Parse(name);
  } else if (parser == ElementH265Parse::GetPluginName()) {
    return new ElementH265Parse(name);
  } else if (parser == ElementMpegParse::GetPluginName()) {
    return new ElementMpegParse(name);
  } else if (parser == ElementTsParse::GetPluginName()) {
    return new ElementTsParse(name);
  }

  NOTREACHED() << "Please register new video parser type: " << parser;
  return nullptr;
}

} // namespace parser
} // namespace elements
} // namespace stream
} // namespace iptv_cloud
