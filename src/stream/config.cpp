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

#include "config.h"

namespace iptv_cloud {
namespace stream {

Config::Config(StreamType type, const input_t &input, const output_t &output)
    : type_(type), input_(input), output_(output) {}

StreamType Config::GetType() const { return type_; }

bool Config::Equals(const Config &api) const {
  return type_ == api.type_ && input_ == api.input_ && output_ == api.output_;
}

input_t Config::GetInput() const { return input_; }

void Config::SetInput(const input_t &input) { input_ = input; }

output_t Config::GetOutput() const { return output_; }

void Config::SetOutput(const output_t &output) { output_ = output; }

} // namespace stream
} // namespace iptv_cloud
