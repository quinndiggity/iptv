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

#include <string>

#include <common/convert2string.h>

#include "utils/arg_reader.h"

namespace common {
bool ConvertFromString(const std::string& from, std::string* out) WARN_UNUSED_RESULT;
}

namespace iptv_cloud {
namespace utils {

template <typename T>
bool ArgsGetValue(const ArgsMap& args, const std::string& key, T* out) {
  if (!out) {
    return false;
  }

  for (size_t i = 0; i < args.size(); ++i) {
    const std::string akey = args[i].first;
    if (akey == key) {
      T lout;
      if (!common::ConvertFromString(args[i].second, &lout)) {
        return false;
      }

      *out = lout;
      return true;
    }
  }

  return false;
}

}  // namespace utils
}  // namespace iptv_cloud
