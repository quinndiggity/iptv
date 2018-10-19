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

#include <vector>

#include <common/draw/types.h>

#include "stream/stypes.h"
#include "types.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

struct AudioChannelInfo {
  double rms_dB;
  double peak_dB;
  double decay_dB;
};

struct ImageInfo {
  common::draw::Point x_y;
  common::draw::Size size;
};

struct SoundInfo {
  SoundInfo();
  volume_t volume;
  bool mute;
  std::vector<AudioChannelInfo> channels;
};

struct StreamInfo {
  ImageInfo img;
  SoundInfo sound;
};

struct MosaicImageOptions {
  MosaicImageOptions();
  bool isValid() const;

  common::draw::Size screen_size;
  int right_padding;
  std::vector<StreamInfo> sreams;
};

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
