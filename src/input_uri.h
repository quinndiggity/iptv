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

#include "types.h"

#define FIELD_INPUT_ID "id"
#define FIELD_INPUT_URI "uri"

#define FIELD_RELAY_AUDIO "relay_audio"
#define FIELD_RELAY_VIDEO "relay_video"
#define FIELD_MUTE_AUDIO "mute"
#define FIELD_VOLUME_AUDIO "volume"

#define TEST_URL "test"

#define DEFAULT_INPUTURI_AUDIO_VOLUME 1.0
#define DEFAULT_INPUTURI_AUDIO_MUTE 0

namespace iptv_cloud {

class InputUri {
 public:
  InputUri();
  explicit InputUri(stream_id_t id, const common::uri::Url& input);

  bool GetRelayVideo() const;
  void SetRelayVideo(bool rv);

  bool GetRelayAudio() const;
  void SetRelayAudio(bool ra);

  stream_id_t GetID() const;
  void SetID(stream_id_t id);

  common::uri::Url GetInput() const;
  void SetInput(const common::uri::Url& uri);

  bool GetMute() const;
  void SetMute(bool mute);

  volume_t GetVolume() const;  // 0.0, 10.0
  void SetVolume(volume_t vol);

  bool Equals(const InputUri& inf) const;

 private:
  stream_id_t id_;
  common::uri::Url input_;

  volume_t volume_;
  bool mute_;

  bool relay_video_;
  bool relay_audio_;
};

inline bool operator==(const InputUri& left, const InputUri& right) {
  return left.Equals(right);
}

inline bool operator!=(const InputUri& left, const InputUri& right) {
  return !operator==(left, right);
}

bool IsTestUrl(const InputUri& url);

}  // namespace iptv_cloud

namespace common {
std::string ConvertToString(const iptv_cloud::InputUri& value);  // json
bool ConvertFromString(const std::string& from, iptv_cloud::InputUri* out);
}  // namespace common
