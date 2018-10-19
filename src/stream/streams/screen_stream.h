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

#include "stream/ibase_stream.h"

#include "stream/streams/configs/audio_video_config.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

class ScreenStream : public IBaseStream {  // only videotestsrc and audiotestsrc
 public:
  ScreenStream(AudioVideoConfig* config, IStreamClient* client, StreamStruct* stats);
  virtual const char* ClassName() const override;

 protected:
  virtual void OnInpudSrcPadCreated(common::uri::Url::scheme scheme, pad::Pad* src_pad, element_id_t id) override;
  virtual void OnOutputSinkPadCreated(common::uri::Url::scheme scheme, pad::Pad* sink_pad, element_id_t id) override;

  virtual IBaseBuilder* CreateBuilder() override;

  virtual void PreLoop() override;
  virtual void PostLoop(ExitStatus status) override;
};

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
