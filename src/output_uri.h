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

#include <common/uri/url.h>

#include <common/file_system/path.h>

#include "types.h"

#define FIELD_OUTPUT_ID "id"
#define FIELD_OUTPUT_URI "uri"
#define FIELD_OUTPUT_HTTP_ROOT "http_root"
#define FIELD_OUTPUT_WIDTH "width"
#define FIELD_OUTPUT_HEIGHT "height"
#define FIELD_OUTPUT_VIDEO_BITRATE "video_bitrate"
#define FIELD_OUTPUT_AUDIO_BITRATE "audio_bitrate"

namespace iptv_cloud {

class OutputUri {
 public:
  typedef common::file_system::ascii_directory_string_path http_root_t;
  OutputUri();
  explicit OutputUri(stream_id_t id, const common::uri::Url& output);

  stream_id_t GetID() const;
  void SetID(stream_id_t id);

  common::uri::Url GetOutput() const;
  void SetOutput(const common::uri::Url& uri);

  http_root_t GetHttpRoot() const;
  void SetHttpRoot(const http_root_t& root);

  int GetWidth() const;
  void SetWidth(int width);

  int GetHeight() const;
  void SetHeight(int height);

  int GetAudioBitrate() const;
  void SetAudioBitrate(int rate);

  int GetVideoBitrate() const;
  void SetVideoBitrate(int rate);

  bool Equals(const OutputUri& inf) const;

 private:
  stream_id_t id_;
  common::uri::Url output_;
  http_root_t http_root_;

  int width_;
  int height_;

  int audio_bitrate_;
  int video_bitrate_;
};

inline bool operator==(const OutputUri& left, const OutputUri& right) {
  return left.Equals(right);
}

inline bool operator!=(const OutputUri& left, const OutputUri& right) {
  return !operator==(left, right);
}

}  // namespace iptv_cloud

namespace common {
std::string ConvertToString(const iptv_cloud::OutputUri& value);  // json
bool ConvertFromString(const std::string& from, iptv_cloud::OutputUri* out);
}  // namespace common
