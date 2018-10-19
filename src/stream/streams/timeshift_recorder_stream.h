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

#include "stream/streams/itimeshift_recorder_stream.h"

#include "utils/chunk_info.h"

namespace iptv_cloud {
namespace stream {
namespace elements {
namespace sink {
class ElementSplitMuxSink;
}
}  // namespace elements
namespace streams {

namespace builders {
class TimeShiftRecorderStreamBuilder;
}

class TimeShiftRecorderStream : public ITimeShiftRecorderStream {
  friend class builders::TimeShiftRecorderStreamBuilder;

 public:
  typedef ITimeShiftRecorderStream base_class;
  TimeShiftRecorderStream(TimeshiftConfig* config,
                          const TimeShiftInfo& info,
                          IStreamClient* client,
                          StreamStruct* stats);
  virtual const char* ClassName() const override;
  virtual ~TimeShiftRecorderStream();

 protected:
  virtual void OnSplitmuxsinkCreated(Connector conn, elements::sink::ElementSplitMuxSink* sink);
  virtual chunk_index_t GetNextChunkStrategy(chunk_index_t last_index, time_t last_index_created_time) const override;

  virtual IBaseBuilder* CreateBuilder() override;

  virtual void HandleDecodeBinElementAdded(GstBin* bin, GstElement* element) override;

  virtual gboolean HandleMainTimerTick() override;
  virtual void OnOutputDataFailed() override;
  virtual gchararray OnPathSet(GstElement* splitmux, guint fragment_id, GstSample* sample);

  chunk_index_t CalcNextIndex() const;
  utils::ChunkInfo chunk_;

 private:
  static gchararray path_setter_callback(GstElement* splitmux, guint fragment_id, gpointer user_data);
  static gchararray path_setter_full_callback(GstElement* splitmux,
                                              guint fragment_id,
                                              GstSample* sample,
                                              gpointer user_data);

  pad::Pad* audio_pad_;
  pad::Pad* video_pad_;
};

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
