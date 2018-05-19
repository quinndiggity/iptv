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

#include <common/serializer/json_serializer.h>

#define STATE_SERVICE_INFO_JOBS_DIRECTORY_FIELD "jobs_directory"
#define STATE_SERVICE_INFO_TIMESHIFTS_DIRECTORY_FIELD "timeshifts_directory"
#define STATE_SERVICE_INFO_HLS_DIRECTORY_FIELD "hls_directory"
#define STATE_SERVICE_INFO_ADS_DIRECTORY_FIELD "ads_directory"
#define STATE_SERVICE_INFO_PLAYLIST_DIRECTORY_FIELD "playlists_directory"
#define STATE_SERVICE_INFO_DVB_DIRECTORY_FIELD "dvb_directory"
#define STATE_SERVICE_INFO_CAPTURE_CARD_DIRECTORY_FIELD "capture_card_directory"

namespace iptv_cloud {
namespace server {

class StateServiceInfo : public common::serializer::JsonSerializer<StateServiceInfo> {
 public:
  typedef JsonSerializer<StateServiceInfo> base_class;
  StateServiceInfo();

  std::string GetJobsDirectory() const;
  std::string GetTimeshiftsDirectory() const;
  std::string GetHlsDirectory() const;
  std::string GetAdsDirectory() const;
  std::string GetPlaylistsDirectory() const;
  std::string GetDvbDirectory() const;
  std::string GetCaptureDirectory() const;

 protected:
  virtual common::Error DoDeSerialize(json_object* serialized) override;
  virtual common::Error SerializeFields(json_object* out) const override;

 private:
  std::string jobs_directory_;
  std::string timeshifts_directory_;
  std::string hls_directory_;
  std::string ads_directory_;
  std::string playlists_directory_;
  std::string dvb_directory_;
  std::string capture_card_directory_;
};

}  // namespace server
}  // namespace iptv_cloud
