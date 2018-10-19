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

#include "server/commands_info/state_service_info.h"

namespace iptv_cloud {
namespace server {

StateServiceInfo::StateServiceInfo()
    : base_class(),
      jobs_directory_(),
      timeshifts_directory_(),
      hls_directory_(),
      ads_directory_(),
      playlists_directory_(),
      dvb_directory_(),
      capture_card_directory_() {}

std::string StateServiceInfo::GetJobsDirectory() const {
  return jobs_directory_;
}

std::string StateServiceInfo::GetTimeshiftsDirectory() const {
  return timeshifts_directory_;
}

std::string StateServiceInfo::GetHlsDirectory() const {
  return hls_directory_;
}

std::string StateServiceInfo::GetAdsDirectory() const {
  return ads_directory_;
}

std::string StateServiceInfo::GetPlaylistsDirectory() const {
  return playlists_directory_;
}

std::string StateServiceInfo::GetDvbDirectory() const {
  return dvb_directory_;
}

std::string StateServiceInfo::GetCaptureDirectory() const {
  return capture_card_directory_;
}

common::Error StateServiceInfo::SerializeFields(json_object* out) const {
  json_object_object_add(out, STATE_SERVICE_INFO_JOBS_DIRECTORY_FIELD, json_object_new_string(jobs_directory_.c_str()));
  json_object_object_add(out, STATE_SERVICE_INFO_TIMESHIFTS_DIRECTORY_FIELD,
                         json_object_new_string(timeshifts_directory_.c_str()));
  json_object_object_add(out, STATE_SERVICE_INFO_HLS_DIRECTORY_FIELD, json_object_new_string(hls_directory_.c_str()));
  json_object_object_add(out, STATE_SERVICE_INFO_ADS_DIRECTORY_FIELD, json_object_new_string(ads_directory_.c_str()));
  json_object_object_add(out, STATE_SERVICE_INFO_PLAYLIST_DIRECTORY_FIELD,
                         json_object_new_string(playlists_directory_.c_str()));
  json_object_object_add(out, STATE_SERVICE_INFO_DVB_DIRECTORY_FIELD, json_object_new_string(dvb_directory_.c_str()));
  json_object_object_add(out, STATE_SERVICE_INFO_CAPTURE_CARD_DIRECTORY_FIELD,
                         json_object_new_string(capture_card_directory_.c_str()));
  return common::Error();
}

common::Error StateServiceInfo::DoDeSerialize(json_object* serialized) {
  StateServiceInfo inf;
  json_object* jjobs_directory = NULL;
  json_bool jjobs_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_JOBS_DIRECTORY_FIELD, &jjobs_directory);
  if (jjobs_directory_exists) {
    inf.jobs_directory_ = json_object_get_string(jjobs_directory);
  }

  json_object* jtimeshifts_directory = NULL;
  json_bool jtimeshifts_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_TIMESHIFTS_DIRECTORY_FIELD, &jtimeshifts_directory);
  if (jtimeshifts_directory_exists) {
    inf.timeshifts_directory_ = json_object_get_string(jtimeshifts_directory);
  }

  json_object* jhls_directory = NULL;
  json_bool jhls_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_HLS_DIRECTORY_FIELD, &jhls_directory);
  if (jhls_directory_exists) {
    inf.hls_directory_ = json_object_get_string(jhls_directory);
  }

  json_object* jads_directory = NULL;
  json_bool jads_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_HLS_DIRECTORY_FIELD, &jads_directory);
  if (jads_directory_exists) {
    inf.ads_directory_ = json_object_get_string(jads_directory);
  }

  json_object* jplaylists_directory = NULL;
  json_bool jplaylists_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_PLAYLIST_DIRECTORY_FIELD, &jplaylists_directory);
  if (jplaylists_directory_exists) {
    inf.playlists_directory_ = json_object_get_string(jplaylists_directory);
  }

  json_object* jdvb_directory = NULL;
  json_bool jdvb_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_DVB_DIRECTORY_FIELD, &jdvb_directory);
  if (jdvb_directory_exists) {
    inf.dvb_directory_ = json_object_get_string(jdvb_directory);
  }

  json_object* jcapture_card_directory = NULL;
  json_bool jcapture_card_directory_exists =
      json_object_object_get_ex(serialized, STATE_SERVICE_INFO_CAPTURE_CARD_DIRECTORY_FIELD, &jcapture_card_directory);
  if (jcapture_card_directory_exists) {
    inf.capture_card_directory_ = json_object_get_string(jcapture_card_directory);
  }

  *this = inf;
  return common::Error();
}

}  // namespace server
}  // namespace iptv_cloud
