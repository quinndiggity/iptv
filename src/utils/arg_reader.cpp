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

#include "utils/arg_reader.h"

namespace iptv_cloud {
namespace utils {

std::pair<std::string, std::string> GetKeyValue(const std::string& line, char separator) {
  const size_t pos = line.find(separator);
  if (pos != std::string::npos) {
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    return std::make_pair(key, value);
  }

  return std::make_pair(line, std::string());
}

ArgsMap ParseCmd(int argc, char** argv) {
  if (argc == 1) {
    return ArgsMap();
  }

  ArgsMap args;
  for (int i = 1; i < argc; ++i) {
    std::string tmp = argv[i];
    std::pair<std::string, std::string> pair = GetKeyValue(tmp, '=');
    args.push_back(pair);
  }
  return args;
}
}  // namespace utils
}  // namespace iptv_cloud
