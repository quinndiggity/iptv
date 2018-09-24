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

#include "stream/dumpers/dumpers_factory.h"

#include <common/string_util.h>

#include "stream/dumpers/htmldump.h"

#define HTML_FORMAT "html"

namespace iptv_cloud {
namespace stream {
namespace dumper {

IDumper *DumpersFactory::CreateDumper(
    const common::file_system::ascii_file_string_path &path) {
  if (!path.IsValid()) {
    return nullptr;
  }

  std::string ext = path.GetExtension();
  if (common::EqualsASCII(ext, HTML_FORMAT, false)) {
    return new HtmlDump;
  }

  return new HtmlDump;
}

} // namespace dumper
} // namespace stream
} // namespace iptv_cloud
