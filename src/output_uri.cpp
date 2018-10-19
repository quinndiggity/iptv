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

#include "output_uri.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <common/sprintf.h>

#include "stream/constants.h"

namespace iptv_cloud {

OutputUri::OutputUri()
    : id_(0),
      output_(),
      http_root_(),
      width_(DEFAULT_VIDEO_WIDTH),
      height_(DEFAULT_VIDEO_HEIGHT),
      audio_bitrate_(DEFAULT_AUDIO_BITRATE),
      video_bitrate_(DEFAULT_VIDEO_BITRATE) {}

OutputUri::OutputUri(stream_id_t id, const common::uri::Url& output)
    : id_(id),
      output_(output),
      http_root_(),
      width_(DEFAULT_VIDEO_WIDTH),
      height_(DEFAULT_VIDEO_HEIGHT),
      audio_bitrate_(DEFAULT_AUDIO_BITRATE),
      video_bitrate_(DEFAULT_VIDEO_BITRATE) {}

stream_id_t OutputUri::GetID() const {
  return id_;
}

void OutputUri::SetID(stream_id_t id) {
  id_ = id;
}

common::uri::Url OutputUri::GetOutput() const {
  return output_;
}

void OutputUri::SetOutput(const common::uri::Url& uri) {
  output_ = uri;
}

OutputUri::http_root_t OutputUri::GetHttpRoot() const {
  return http_root_;
}

void OutputUri::SetHttpRoot(const http_root_t& root) {
  http_root_ = root;
}

int OutputUri::GetWidth() const {
  return width_;
}

void OutputUri::SetWidth(int width) {
  width_ = width;
}

int OutputUri::GetHeight() const {
  return height_;
}

void OutputUri::SetHeight(int height) {
  height_ = height;
}

int OutputUri::GetAudioBitrate() const {
  return audio_bitrate_;
}

void OutputUri::SetAudioBitrate(int rate) {
  audio_bitrate_ = rate;
}

int OutputUri::GetVideoBitrate() const {
  return video_bitrate_;
}

void OutputUri::SetVideoBitrate(int rate) {
  video_bitrate_ = rate;
}

bool OutputUri::Equals(const OutputUri& inf) const {
  return id_ == inf.id_ && output_ == inf.output_ && http_root_ == inf.http_root_ && width_ == inf.width_ &&
         height_ == inf.height_ && audio_bitrate_ == inf.audio_bitrate_ && video_bitrate_ == inf.video_bitrate_;
}

}  // namespace iptv_cloud

namespace common {

namespace {
const std::string hls_type_str[] = {"none", "pull", "push"};
const std::string hls_out_type_str[] = {"main", "adaptive"};
}  // namespace

std::string ConvertToString(const iptv_cloud::OutputUri& value) {
  common::file_system::ascii_directory_string_path ps = value.GetHttpRoot();
  const std::string http_root_str = ps.GetPath();
  return common::MemSPrintf("{ \"" FIELD_OUTPUT_ID "\": %llu, \"" FIELD_OUTPUT_URI
                            "\": \"%s\", \"" FIELD_OUTPUT_HTTP_ROOT "\": \"%s\", \"" FIELD_OUTPUT_WIDTH
                            "\": %d, \"" FIELD_OUTPUT_HEIGHT "\": %d, \"" FIELD_OUTPUT_VIDEO_BITRATE
                            "\": %d, \"" FIELD_OUTPUT_AUDIO_BITRATE "\": %d }",
                            value.GetID(), common::ConvertToString(value.GetOutput()), http_root_str, value.GetWidth(),
                            value.GetHeight(), value.GetVideoBitrate(), value.GetAudioBitrate());
}

bool ConvertFromString(const std::string& from, iptv_cloud::OutputUri* out) {
  if (!out) {
    return false;
  }

  json_object* obj = json_tokener_parse(from.c_str());
  if (!obj) {
    return false;
  }

  iptv_cloud::OutputUri res;
  json_object* jid = NULL;
  json_bool jid_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_ID, &jid);
  if (jid_exists) {
    res.SetID(json_object_get_int64(jid));
  }

  json_object* juri = NULL;
  json_bool juri_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_URI, &juri);
  if (juri_exists) {
    res.SetOutput(common::uri::Url(json_object_get_string(juri)));
  }

  json_object* jhttp_root = NULL;
  json_bool jhttp_root_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_HTTP_ROOT, &jhttp_root);
  if (jhttp_root_exists) {
    const char* http_root_str = json_object_get_string(jhttp_root);
    const common::file_system::ascii_directory_string_path http_root(http_root_str);
    res.SetHttpRoot(http_root);
  }

  json_object* jwidth = NULL;
  json_bool jwidth_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_WIDTH, &jwidth);
  if (jwidth_exists) {
    res.SetWidth(json_object_get_int(jwidth));
  }

  json_object* jheight = NULL;
  json_bool jheight_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_HEIGHT, &jheight);
  if (jheight_exists) {
    res.SetHeight(json_object_get_int(jheight));
  }

  json_object* jvbitrate = NULL;
  json_bool jvbitrate_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_VIDEO_BITRATE, &jvbitrate);
  if (jvbitrate_exists) {
    res.SetVideoBitrate(json_object_get_int(jvbitrate));
  }

  json_object* jabitrate = NULL;
  json_bool jabitrate_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_AUDIO_BITRATE, &jabitrate);
  if (jabitrate_exists) {
    res.SetAudioBitrate(json_object_get_int(jabitrate));
  }

  json_object_put(obj);
  *out = res;
  return true;
}

}  // namespace common
