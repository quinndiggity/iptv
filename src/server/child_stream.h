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

#include "protocol/types.h"

#include "types.h"

#include <common/libev/io_child.h>

namespace iptv_cloud {
struct StreamStruct;
namespace server {
namespace pipe {
class PipeClient;
}

class ChildStream : public common::libev::IoChild {
 public:
  typedef common::libev::IoChild base_class;
  ChildStream(common::libev::IoLoop* server, StreamStruct* mem);
  ~ChildStream();

  common::ErrnoError SendStop(protocol::sequance_id_t id) WARN_UNUSED_RESULT;
  common::ErrnoError SendRestart(protocol::sequance_id_t id) WARN_UNUSED_RESULT;

  channel_id_t GetChannelId() const;

  bool Equals(const ChildStream& stream) const;

  pipe::PipeClient* GetPipe() const;
  void SetPipe(pipe::PipeClient* pipe);

 private:
  DISALLOW_COPY_AND_ASSIGN(ChildStream);

  StreamStruct* mem_;
  pipe::PipeClient* pipe_client_;
};

inline bool operator==(const ChildStream& left, const ChildStream& right) {
  return left.Equals(right);
}

inline bool operator!=(const ChildStream& left, const ChildStream& right) {
  return !operator==(left, right);
}

}  // namespace server
}  // namespace iptv_cloud
