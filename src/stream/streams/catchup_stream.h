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

#include "stream/streams/timeshift_recorder_stream.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

class CatchupStream : public TimeShiftRecorderStream {
 public:
  typedef TimeShiftRecorderStream base_class;
  CatchupStream(TimeshiftConfig* config, const TimeShiftInfo& info, IStreamClient* client, StreamStruct* stats);

  virtual const char* ClassName() const override;

 protected:
  virtual chunk_index_t GetNextChunkStrategy(chunk_index_t last_index, time_t last_index_created_time) const override;
  virtual IBaseBuilder* CreateBuilder() override;

  virtual void PostLoop(ExitStatus status) override;
  virtual gchararray OnPathSet(GstElement* splitmux, guint fragment_id, GstSample* sample) override;

 private:
  void WriteM3u8List();
  std::vector<utils::ChunkInfo> chunks_;
};

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
