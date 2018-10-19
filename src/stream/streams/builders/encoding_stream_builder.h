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

#include "stream/streams/builders/src_decodebin_stream_builder.h"

#include "stream/streams/configs/encoding_config.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
class ElementQueue;
}
namespace streams {
namespace builders {

class EncodingStreamBuilder : public SrcDecodeStreamBuilder {
 public:
  EncodingStreamBuilder(EncodingConfig* api, SrcDecodeBinStream* observer);
  virtual Connector BuildPostProc(Connector conn) override;
  virtual Connector BuildConverter(Connector conn) override;

  virtual SupportedVideoCodecs GetVideoCodecType() const override;
  virtual SupportedAudioCodecs GetAudioCodecType() const override;

 protected:
  virtual elements_line_t BuildVideoPostProc(element_id_t video_id);
  virtual elements_line_t BuildAudioPostProc(element_id_t audio_id);

  virtual elements_line_t BuildVideoConverter(element_id_t video_id);
  virtual elements_line_t BuildAudioConverter(element_id_t audio_id);
};

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
