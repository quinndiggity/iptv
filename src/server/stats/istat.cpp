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

#include "server/stats/istat.h"

#ifdef HAVE_REDIS
#include "redis/stat.h"
#endif
#include "fake/stat.h"

namespace iptv_cloud {
namespace server {
namespace stats {

StatCredentialsBase *IStat::GetCreds() const { return creds_.get(); }

IStat::IStat(StatCredentialsBase *creds) : creds_(creds) {}

IStat::~IStat() {}

IStat *IStat::CreateStat(StatCredentialsBase *creds) {
  if (!creds) {
    return nullptr;
  }
#ifdef HAVE_REDIS
  if (creds->GetType() == STAT_REDIS) {
    return new redis::Stat(static_cast<redis::StatCredentials *>(creds));
  }
#endif
  return new fake::Stat(static_cast<fake::StatCredentials *>(creds));
}

} // namespace stats
} // namespace server
} // namespace iptv_cloud
