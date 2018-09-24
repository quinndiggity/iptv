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

#include "server/pipe/pipe_client.h"

namespace iptv_cloud {
namespace server {
namespace pipe {

PipeClient::PipeClient(common::libev::IoLoop *server, descriptor_t read_fd,
                       descriptor_t write_fd)
    : common::libev::IoClient(server),
      pipe_read_client_(new common::libev::PipeReadClient(nullptr, read_fd)),
      pipe_write_client_(new common::libev::PipeWriteClient(nullptr, write_fd)),
      read_fd_(read_fd) {}

common::ErrnoError PipeClient::Write(const void *data, size_t size,
                                     size_t *nwrite_out) {
  return pipe_write_client_->Write(data, size, nwrite_out);
}

common::ErrnoError PipeClient::Read(char *out, size_t max_size, size_t *nread) {
  return pipe_read_client_->Read(out, max_size, nread);
}

common::ErrnoError PipeClient::Read(unsigned char *out, size_t max_size,
                                    size_t *nread) {
  return pipe_read_client_->Read(out, max_size, nread);
}

descriptor_t PipeClient::GetFd() const { return read_fd_; }

common::ErrnoError PipeClient::DoClose() {
  pipe_write_client_->Close();
  pipe_read_client_->Close();
  return common::ErrnoError();
}

const char *PipeClient::ClassName() const { return "PipeClient"; }

PipeClient::~PipeClient() {
  destroy(&pipe_write_client_);
  destroy(&pipe_read_client_);
}

} // namespace pipe
} // namespace server
} // namespace iptv_cloud
