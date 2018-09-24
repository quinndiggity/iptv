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

#include <common/patterns/singleton_pattern.h>

#include "stream/ibase_stream.h"

#include "timeshift.h" // for invalid_chunk_index, TimeShiftInfo (ptr...

namespace iptv_cloud {
namespace stream {

class StreamsFactory : public common::patterns::LazySingleton<StreamsFactory> {
public:
  friend class common::patterns::LazySingleton<StreamsFactory>;

  IBaseStream *CreateStream(const utils::ArgsMap &args,
                            IBaseStream::IStreamClient *client,
                            StreamStruct *stats,
                            chunk_index_t start_chunk_index);
};

bool IsTimeshiftPlayer(const utils::ArgsMap &args, TimeShiftInfo *tinfo);

} // namespace stream
} // namespace iptv_cloud
