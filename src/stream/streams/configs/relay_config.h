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

#include "stream/streams/configs/audio_video_config.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

class RelayConfig : public AudioVideoConfig {
 public:
  typedef AudioVideoConfig base_class;
  explicit RelayConfig(const base_class& config);

  std::string GetVideoParser() const;  // relay
  void SetVideoParser(const std::string& parser);

  std::string GetAudioParser() const;  // relay
  void SetAudioParser(const std::string& parser);

 private:
  std::string video_parser_;
  std::string audio_parser_;
};

class TimeshiftConfig : public RelayConfig {
 public:
  typedef RelayConfig base_class;
  explicit TimeshiftConfig(const base_class& config);

  time_t GetTimeShiftChunkDuration() const;  // timeshift_rec, catchup_rec
  void SetTimeShiftChunkDuration(time_t t);  // timeshift_rec, catchup_rec

 private:
  time_t timeshift_chunk_duration_;
};

class PlaylistRelayConfig : public RelayConfig {
 public:
  typedef RelayConfig base_class;
  explicit PlaylistRelayConfig(const base_class& config);

  bool GetLoop() const;  // enc_playlist relay_playlist
  void SetLoop(bool loop);

 private:
  bool loop_;
};

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
