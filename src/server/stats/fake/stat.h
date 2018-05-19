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

#include "server/stats/istat.h"  // for IStat

namespace iptv_cloud {
namespace server {
namespace stats {
namespace fake {

class StatCredentials : public StatCredentialsBase {
 public:
  StatCredentials();
};

class Stat : public IStat {
 public:
  Stat(StatCredentials* creds);  // take ownership

  virtual bool SetKey(const std::string& key, const std::string& value) override;
  virtual bool GetKey(const std::string& key, std::string* value) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(Stat);
  const std::unique_ptr<StatCredentials> creds_;
};

}  // namespace fake
}  // namespace stats
}  // namespace server
}  // namespace iptv_cloud
