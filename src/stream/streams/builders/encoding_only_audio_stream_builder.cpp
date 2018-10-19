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

#include "stream/streams/builders/encoding_only_audio_stream_builder.h"

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

EncodingOnlyAudioStreamBuilder::EncodingOnlyAudioStreamBuilder(EncodingConfig* api, SrcDecodeBinStream* observer)
    : EncodingStreamBuilder(api, observer) {}

elements_line_t EncodingOnlyAudioStreamBuilder::BuildVideoPostProc(element_id_t video_id) {
  UNUSED(video_id);
  return {nullptr, nullptr};
}

elements_line_t EncodingOnlyAudioStreamBuilder::BuildVideoConverter(element_id_t video_id) {
  UNUSED(video_id);
  return {nullptr, nullptr};
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
