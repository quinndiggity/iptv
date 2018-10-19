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

#include "server/commands_info/stop_service_info.h"

#define STOP_SERVICE_INFO_DELAY_FIELD "delay"

namespace iptv_cloud {
namespace server {

StopServiceInfo::StopServiceInfo() : base_class(), delay_(0) {}

StopServiceInfo::StopServiceInfo(const std::string& license, common::time64_t delay)
    : base_class(license), delay_(delay) {}

common::Error StopServiceInfo::SerializeFields(json_object* obj) const {
  json_object_object_add(obj, STOP_SERVICE_INFO_DELAY_FIELD, json_object_new_int64(delay_));
  return base_class::SerializeFields(obj);
}

common::Error StopServiceInfo::DoDeSerialize(json_object* serialized) {
  StopServiceInfo inf;
  common::Error err = inf.base_class::DoDeSerialize(serialized);
  UNUSED(err);

  json_object* jlicense = NULL;
  json_bool jdelay_exists = json_object_object_get_ex(serialized, STOP_SERVICE_INFO_DELAY_FIELD, &jlicense);
  if (jdelay_exists) {
    inf.delay_ = json_object_get_int64(jlicense);
  }

  *this = inf;
  return common::Error();
}

common::time64_t StopServiceInfo::GetDelay() const {
  return delay_;
}

}  // namespace server
}  // namespace iptv_cloud
