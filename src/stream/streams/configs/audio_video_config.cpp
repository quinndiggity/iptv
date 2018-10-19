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

#include "stream/streams/configs/audio_video_config.h"

#include "stream/constants.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

AudioVideoConfig::AudioVideoConfig(const base_class& config)
    : base_class(config), have_video_(true), have_audio_(true), audio_select_(DEFAULT_AUDIO_SELECT) {}

bool AudioVideoConfig::HaveVideo() const {
  return have_video_;
}

void AudioVideoConfig::SetHaveVideo(bool have_video) {
  have_video_ = have_video;
}

bool AudioVideoConfig::HaveAudio() const {
  return have_audio_;
}

void AudioVideoConfig::SetHaveAudio(bool have_audio) {
  have_audio_ = have_audio;
}

int AudioVideoConfig::GetAudioSelect() const {
  return audio_select_;
}

void AudioVideoConfig::SetAudioSelect(int sel) {
  audio_select_ = sel;
}

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
