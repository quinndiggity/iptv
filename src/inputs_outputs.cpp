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

#include "inputs_outputs.h"

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <common/convert2string.h>

#include "stream/constants.h"  // for INPUT_FIELD

#include "utils/arg_converter.h"

namespace common {

std::string ConvertToString(const iptv_cloud::output_t& value) {
  json_object* output = json_object_new_object();
  json_object* jurls = json_object_new_array();
  for (size_t i = 0; i < value.size(); ++i) {
    iptv_cloud::OutputUri uri = value[i];
    json_object* jurl = json_object_new_object();
    json_object_object_add(jurl, FIELD_OUTPUT_ID, json_object_new_int64(uri.GetID()));
    const std::string url_str = common::ConvertToString(uri.GetOutput());
    json_object_object_add(jurl, FIELD_OUTPUT_URI, json_object_new_string(url_str.c_str()));
    const common::file_system::ascii_directory_string_path http_root = uri.GetHttpRoot();
    const std::string http_root_str = http_root.GetPath();
    json_object_object_add(jurl, FIELD_OUTPUT_HTTP_ROOT, json_object_new_string(http_root_str.c_str()));
    json_object_array_add(jurls, jurl);  // urls
  }
  json_object_object_add(output, FIELD_OUTPUT_URLS, jurls);
  std::string res = json_object_get_string(output);
  json_object_put(output);
  return res;
}

bool ConvertFromString(const std::string& output_urls, iptv_cloud::output_t* out) {
  if (!out) {
    return false;
  }

  json_object* obj = json_tokener_parse(output_urls.c_str());
  if (!obj) {
    return false;
  }

  json_object* jurls = NULL;
  json_bool jurls_exists = json_object_object_get_ex(obj, FIELD_OUTPUT_URLS, &jurls);
  if (!jurls_exists) {
    json_object_put(obj);
    return false;
  }

  iptv_cloud::output_t output;
  int len = json_object_array_length(jurls);
  for (int i = 0; i < len; ++i) {
    json_object* jurl = json_object_array_get_idx(jurls, i);
    const char* uri_str = json_object_get_string(jurl);
    iptv_cloud::OutputUri lurl;
    if (common::ConvertFromString(uri_str, &lurl)) {
      output.push_back(lurl);
    }
  }
  json_object_put(obj);
  *out = output;
  return true;
}

std::string ConvertToString(const iptv_cloud::input_t& value) {
  json_object* input = json_object_new_object();
  json_object* jurls = json_object_new_array();
  for (size_t i = 0; i < value.size(); ++i) {
    iptv_cloud::InputUri uri = value[i];
    json_object* jurl = json_object_new_object();
    json_object_object_add(jurl, FIELD_INPUT_ID, json_object_new_int64(uri.GetID()));
    const std::string url_str = common::ConvertToString(uri.GetInput());
    json_object_object_add(jurl, FIELD_INPUT_URI, json_object_new_string(url_str.c_str()));
    json_object_array_add(jurls, jurl);  // urls
  }
  json_object_object_add(input, FIELD_OUTPUT_URLS, jurls);
  std::string res = json_object_get_string(input);
  json_object_put(input);
  return res;
}

bool ConvertFromString(const std::string& input_urls, iptv_cloud::input_t* out) {
  if (!out) {
    return false;
  }

  json_object* obj = json_tokener_parse(input_urls.c_str());
  if (!obj) {
    return false;
  }

  json_object* jurls = NULL;
  json_bool jurls_exists = json_object_object_get_ex(obj, FIELD_INPUT_URLS, &jurls);
  if (!jurls_exists) {
    json_object_put(obj);
    return false;
  }

  iptv_cloud::input_t input;
  int len = json_object_array_length(jurls);
  for (int i = 0; i < len; ++i) {
    json_object* jurl = json_object_array_get_idx(jurls, i);
    const char* uri_str = json_object_get_string(jurl);
    iptv_cloud::InputUri url;
    if (common::ConvertFromString(uri_str, &url)) {
      input.push_back(url);
    }
  }
  json_object_put(obj);
  *out = input;
  return true;
}

}  // namespace common

namespace iptv_cloud {
bool read_input(const utils::ArgsMap& config, input_t* input) {
  if (!input) {
    return false;
  }

  input_t linput;
  if (!utils::ArgsGetValue(config, INPUT_FIELD, &linput)) {
    return false;
  }

  *input = linput;
  return true;
}

bool read_output(const utils::ArgsMap& config, output_t* output) {
  if (!output) {
    return false;
  }

  output_t loutput;
  if (!utils::ArgsGetValue(config, OUTPUT_FIELD, &loutput)) {
    return false;
  }

  *output = loutput;
  return true;
}

}  // namespace iptv_cloud
