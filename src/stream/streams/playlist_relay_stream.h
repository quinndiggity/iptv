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

#include "stream/streams/relay_stream.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sources {
class ElementAppSrc;
}
} // namespace elements

namespace streams {

namespace builders {
class PlaylistRelayStreamBuilder;
}

class PlaylistRelayStream : public RelayStream {
  friend class builders::PlaylistRelayStreamBuilder;

public:
  PlaylistRelayStream(PlaylistRelayConfig *config, IStreamClient *client,
                      StreamStruct *stats);
  ~PlaylistRelayStream();

  virtual const char *ClassName() const override;

protected:
  virtual void OnAppSrcCreatedCreated(elements::sources::ElementAppSrc *src);

  virtual IBaseBuilder *CreateBuilder() override;

  virtual void PreLoop() override;
  virtual void PostLoop(ExitStatus status) override;

  virtual void HandleNeedData(GstElement *pipeline, guint rsize);

private:
  static void need_data_callback(GstElement *pipeline, guint size,
                                 gpointer user_data);

  FILE *OpenNextFile();

  elements::sources::ElementAppSrc *app_src_;
  FILE *current_file_;
  size_t curent_pos_;
};

} // namespace streams
} // namespace stream
} // namespace iptv_cloud
