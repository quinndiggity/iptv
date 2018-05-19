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

#include "stream/streams/builders/timeshift_recorder_stream_builder.h"

#include <common/sprintf.h>

#include "stream/elements/muxer/muxer.h"
#include "stream/elements/sink/sink.h"

#include "stream/streams/timeshift_recorder_stream.h"

namespace iptv_cloud {
namespace stream {
namespace streams {
namespace builders {

TimeShiftRecorderStreamBuilder::TimeShiftRecorderStreamBuilder(TimeshiftConfig* api, TimeShiftRecorderStream* observer)
    : base_class(api, observer) {}

Connector TimeShiftRecorderStreamBuilder::BuildOutput(Connector conn) {
  elements::sink::ElementSplitMuxSink* splitmuxsink =
      new elements::sink::ElementSplitMuxSink(common::MemSPrintf(SPLIT_SINK_NAME_1U, 0));
  ElementAdd(splitmuxsink);

  TimeshiftConfig* tconf = static_cast<TimeshiftConfig*>(api_);
  elements::muxer::ElementMPEGTSMux* mpegtsmux = elements::muxer::make_mpegtsmux(0);
  guint64 mst_nsec = tconf->GetTimeShiftChunkDuration() * GST_SECOND;
  splitmuxsink->SetMuxer(mpegtsmux);
  delete mpegtsmux;

  splitmuxsink->SetMaxSizeTime(mst_nsec);
  HandleSplitmuxsinkCreated(conn, splitmuxsink);
  return conn;
}

void TimeShiftRecorderStreamBuilder::HandleSplitmuxsinkCreated(Connector conn,
                                                               elements::sink::ElementSplitMuxSink* sink) {
  if (observer_) {
    TimeShiftRecorderStream* ts_observer = static_cast<TimeShiftRecorderStream*>(observer_);
    ts_observer->OnSplitmuxsinkCreated(conn, sink);
  }
}

}  // namespace builders
}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
