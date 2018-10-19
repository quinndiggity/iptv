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

#include <common/macros.h>

namespace iptv_cloud {
namespace server {
namespace stats {
enum StatType {
  STAT_FAKE = 0,
#ifdef HAVE_REDIS
  STAT_REDIS
#endif
};

class StatCredentialsBase {
 public:
  static StatCredentialsBase* CreateCreadentialsFromString(const std::string& json_data) WARN_UNUSED_RESULT;
  static bool IsValidCredentials(const std::string& json_data,
                                 StatCredentialsBase** creds = nullptr) WARN_UNUSED_RESULT;
  StatType GetType() const;

 protected:
  StatCredentialsBase(StatType type);

 private:
  const StatType type_;
};

}  // namespace stats
}  // namespace server
}  // namespace iptv_cloud
