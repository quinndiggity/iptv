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

#include "input_uri.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <common/sprintf.h>

namespace iptv_cloud {

InputUri::InputUri()
    : id_(0), input_(), volume_(DEFAULT_INPUTURI_AUDIO_VOLUME),
      mute_(DEFAULT_INPUTURI_AUDIO_MUTE), relay_video_(false),
      relay_audio_(false) {}

InputUri::InputUri(stream_id_t id, const common::uri::Url &input)
    : id_(id), input_(input), volume_(DEFAULT_INPUTURI_AUDIO_VOLUME),
      mute_(DEFAULT_INPUTURI_AUDIO_MUTE), relay_video_(false),
      relay_audio_(false) {}

bool InputUri::GetRelayVideo() const { return relay_video_; }

void InputUri::SetRelayVideo(bool rv) { relay_video_ = rv; }

bool InputUri::GetRelayAudio() const { return relay_audio_; }

void InputUri::SetRelayAudio(bool ra) { relay_audio_ = ra; }

stream_id_t InputUri::GetID() const { return id_; }

void InputUri::SetID(stream_id_t id) { id_ = id; }

common::uri::Url InputUri::GetInput() const { return input_; }

void InputUri::SetInput(const common::uri::Url &uri) { input_ = uri; }

bool InputUri::GetMute() const { return mute_; }

void InputUri::SetMute(bool mute) { mute_ = mute; }

volume_t InputUri::GetVolume() const { return volume_; }

void InputUri::SetVolume(volume_t vol) { volume_ = vol; }

bool InputUri::Equals(const InputUri &inf) const {
  return id_ == inf.id_ && input_ == inf.input_ && volume_ == inf.volume_ &&
         mute_ == inf.mute_ && relay_video_ == inf.relay_video_ &&
         relay_audio_ == inf.relay_audio_;
}

bool IsTestUrl(const InputUri &url) {
  return url.GetInput() == common::uri::Url(TEST_URL);
}

} // namespace iptv_cloud

namespace common {

std::string ConvertToString(const iptv_cloud::InputUri &value) {
  return common::MemSPrintf(
      "{ \"" FIELD_INPUT_ID "\": %llu, \"" FIELD_INPUT_URI
      "\": \"%s\", \"" FIELD_VOLUME_AUDIO "\": %.2f, \"" FIELD_MUTE_AUDIO
      "\": %d, \"" FIELD_RELAY_VIDEO "\": %d, \"" FIELD_RELAY_AUDIO "\": %d}",
      value.GetID(), common::ConvertToString(value.GetInput()),
      value.GetVolume(), value.GetMute(), value.GetRelayVideo(),
      value.GetRelayAudio());
}

bool ConvertFromString(const std::string &from, iptv_cloud::InputUri *out) {
  if (!out) {
    return false;
  }

  json_object *obj = json_tokener_parse(from.c_str());
  if (!obj) {
    return false;
  }

  iptv_cloud::InputUri res;
  json_object *jid = NULL;
  json_bool jid_exists = json_object_object_get_ex(obj, FIELD_INPUT_ID, &jid);
  if (jid_exists) {
    res.SetID(json_object_get_int64(jid));
  }

  json_object *juri = NULL;
  json_bool juri_exists =
      json_object_object_get_ex(obj, FIELD_INPUT_URI, &juri);
  if (juri_exists) {
    res.SetInput(common::uri::Url(json_object_get_string(juri)));
  }

  json_object *juri_volume_audio = NULL;
  json_bool juri_volume_audio_exists =
      json_object_object_get_ex(obj, FIELD_VOLUME_AUDIO, &juri_volume_audio);
  if (juri_volume_audio_exists) {
    res.SetVolume(json_object_get_double(juri_volume_audio));
  }

  json_object *juri_mute_audio = NULL;
  json_bool juri_mute_audio_exists =
      json_object_object_get_ex(obj, FIELD_MUTE_AUDIO, &juri_mute_audio);
  if (juri_mute_audio_exists) {
    res.SetMute(json_object_get_boolean(juri_mute_audio));
  }

  json_object *juri_relay_video = NULL;
  json_bool juri_relay_video_exists =
      json_object_object_get_ex(obj, FIELD_RELAY_VIDEO, &juri_relay_video);
  if (juri_relay_video_exists) {
    res.SetRelayVideo(json_object_get_boolean(juri_relay_video));
  }

  json_object *juri_relay_audio = NULL;
  json_bool juri_relay_audio_exists =
      json_object_object_get_ex(obj, FIELD_RELAY_AUDIO, &juri_relay_audio);
  if (juri_relay_audio_exists) {
    res.SetRelayAudio(json_object_get_boolean(juri_relay_audio));
  }

  json_object_put(obj);
  *out = res;
  return true;
}

} // namespace common
