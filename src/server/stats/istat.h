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

#include <memory>

// for channel_id_t

#include "server/stats/stat_credentials.h"

namespace iptv_cloud {
namespace server {
namespace stats {

class IStat {
 public:
  virtual bool SetKey(const std::string& key, const std::string& value) = 0;
  virtual bool GetKey(const std::string& key, std::string* value) = 0;

  StatCredentialsBase* GetCreds() const;

  virtual ~IStat();

  static IStat* CreateStat(StatCredentialsBase* creds);

 protected:
  IStat(StatCredentialsBase* creds);

 private:
  const std::unique_ptr<StatCredentialsBase> creds_;
};

}  // namespace stats
}  // namespace server
}  // namespace iptv_cloud
