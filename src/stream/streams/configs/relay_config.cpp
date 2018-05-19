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

#include "stream/streams/configs/relay_config.h"

#include "stream/constants.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

RelayConfig::RelayConfig(const base_class& config)
    : base_class(config), video_parser_(DEFAULT_VIDEO_PARSER), audio_parser_(DEFAULT_AUDIO_PARSER) {}

std::string RelayConfig::GetVideoParser() const {
  return video_parser_;
}

void RelayConfig::SetVideoParser(const std::string& parser) {
  video_parser_ = parser;
}

std::string RelayConfig::GetAudioParser() const {
  return audio_parser_;
}

void RelayConfig::SetAudioParser(const std::string& parser) {
  audio_parser_ = parser;
}

TimeshiftConfig::TimeshiftConfig(const base_class& config)
    : base_class(config), timeshift_chunk_duration_(DEFAULT_TIMESHIFT_CHUNK_DURATION) {}

time_t TimeshiftConfig::GetTimeShiftChunkDuration() const {
  return timeshift_chunk_duration_;
}

void TimeshiftConfig::SetTimeShiftChunkDuration(time_t t) {
  timeshift_chunk_duration_ = t;
}

PlaylistRelayConfig::PlaylistRelayConfig(const base_class& config) : base_class(config), loop_(false) {}

void PlaylistRelayConfig::SetLoop(bool loop) {
  loop_ = loop;
}

bool PlaylistRelayConfig::GetLoop() const {
  return loop_;
}

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
