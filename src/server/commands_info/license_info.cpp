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

#include "server/commands_info/license_info.h"

#define LICENSE_INFO_KEY_FIELD "license_key"

namespace iptv_cloud {
namespace server {

LicenseInfo::LicenseInfo() : base_class(), license_() {}

LicenseInfo::LicenseInfo(const std::string& license) : base_class(), license_(license) {}

common::Error LicenseInfo::SerializeFields(json_object* out) const {
  json_object_object_add(out, LICENSE_INFO_KEY_FIELD, json_object_new_string(license_.c_str()));
  return common::Error();
}

common::Error LicenseInfo::DoDeSerialize(json_object* serialized) {
  LicenseInfo inf;
  json_object* jlicense = NULL;
  json_bool jlicense_exists = json_object_object_get_ex(serialized, LICENSE_INFO_KEY_FIELD, &jlicense);
  if (jlicense_exists) {
    inf.license_ = json_object_get_string(jlicense);
  }

  *this = inf;
  return common::Error();
}

std::string LicenseInfo::GetLicense() const {
  return license_;
}

}  // namespace server
}  // namespace iptv_cloud
