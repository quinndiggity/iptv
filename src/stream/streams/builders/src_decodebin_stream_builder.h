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

#include "stream/streams/builders/gst_base_builder.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
class ElementDecodebin;
}
namespace streams {
class SrcDecodeBinStream;
namespace builders {

class SrcDecodeStreamBuilder : public GstBaseBuilder {
public:
  SrcDecodeStreamBuilder(Config *api, SrcDecodeBinStream *observer);

  virtual Connector BuildInput() override;
  virtual elements::Element *BuildInputSrc();

  virtual Connector BuildUdbConnections(Connector conn) override;
  virtual elements::Element *BuildVideoUdbConnection();
  virtual elements::Element *BuildAudioUdbConnection();

  virtual Connector BuildPostProc(Connector conn) override = 0;
  virtual Connector BuildConverter(Connector conn) override = 0;
  virtual Connector BuildOutput(Connector conn) override;

  virtual SupportedVideoCodecs GetVideoCodecType() const = 0;
  virtual SupportedAudioCodecs GetAudioCodecType() const = 0;

protected:
  void HandleDecodebinCreated(elements::ElementDecodebin *decodebin);
};

} // namespace builders
} // namespace streams
} // namespace stream
} // namespace iptv_cloud
