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

#include "stream_commands_info/changed_sources_info.h"

#define CHANGE_SOURCES_URL_FIELD "url"

namespace iptv_cloud {

ChangedSouresInfo::ChangedSouresInfo(const url_t &url)
    : base_class(), url_(url) {}

ChangedSouresInfo::ChangedSouresInfo() : base_class(), url_() {}

common::Error ChangedSouresInfo::SerializeFields(json_object *out) const {
  std::string url_str = common::ConvertToString(url_);
  json_object_object_add(out, CHANGE_SOURCES_URL_FIELD,
                         json_object_new_string(url_str.c_str()));
  return common::Error();
}

ChangedSouresInfo::url_t ChangedSouresInfo::GetUrl() const { return url_; }

common::Error ChangedSouresInfo::DoDeSerialize(json_object *serialized) {
  ChangedSouresInfo inf;
  json_object *jurl = NULL;
  json_bool jurl_exists =
      json_object_object_get_ex(serialized, CHANGE_SOURCES_URL_FIELD, &jurl);
  if (jurl_exists) {
    std::string url_str = json_object_get_string(jurl);
    url_t lurl;
    if (common::ConvertFromString(url_str, &lurl)) {
      inf.url_ = lurl;
    }
  }

  *this = inf;
  return common::Error();
}

} // namespace iptv_cloud
