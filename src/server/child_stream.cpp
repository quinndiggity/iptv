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

#include "server/child_stream.h"

#include "stream_struct_utils.h"

#include "pipe/pipe_client.h"

#include "stream_commands.h"

namespace iptv_cloud {
namespace server {

ChildStream::ChildStream(common::libev::IoLoop *server, StreamStruct *mem)
    : base_class(server), mem_(mem), pipe_client_(nullptr) {}

ChildStream::~ChildStream() { FreeSharedStreamStruct(&mem_); }

channel_id_t ChildStream::GetChannelId() const { return mem_->id; }

bool ChildStream::Equals(const ChildStream &stream) const {
  return mem_->id == stream.mem_->id;
}

pipe::PipeClient *ChildStream::GetPipe() const { return pipe_client_; }

void ChildStream::SetPipe(pipe::PipeClient *pipe) { pipe_client_ = pipe; }

common::ErrnoError ChildStream::SendStop(protocol::sequance_id_t id) {
  if (!pipe_client_) {
    return common::make_errno_error_inval();
  }

  pipe::ProtocoledPipeClient *pipe =
      static_cast<pipe::ProtocoledPipeClient *>(pipe_client_);
  protocol::request_t req = StopStreamRequest(id, "{}");
  return pipe->WriteRequest(req);
}

common::ErrnoError ChildStream::SendRestart(protocol::sequance_id_t id) {
  if (!pipe_client_) {
    return common::make_errno_error_inval();
  }

  pipe::ProtocoledPipeClient *pipe =
      static_cast<pipe::ProtocoledPipeClient *>(pipe_client_);
  protocol::request_t req = RestartStreamRequest(id, "{}");
  return pipe->WriteRequest(req);
}

} // namespace server
} // namespace iptv_cloud
