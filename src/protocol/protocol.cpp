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

#include "protocol/protocol.h"

#include <common/sprintf.h>
#include <common/sys_byteorder.h>

namespace iptv_cloud {
namespace protocol {

namespace {
common::Error GenerateProtocoledMessage(const std::string& message, char** data, size_t* data_len) {
  if (message.empty() || !data || !data_len) {
    return common::make_error_inval();
  }

  const char* data_ptr = message.data();
  const size_t size = message.size();

  const protocoled_size_t data_size = size;
  if (data_size > MAX_COMMAND_SIZE) {
    return common::make_error(common::MemSPrintf("Reached limit of command size: %u", data_size));
  }

  const protocoled_size_t message_size = common::HostToNet32(data_size);  // stable
  const size_t protocoled_data_len = size + sizeof(protocoled_size_t);

  char* protocoled_data = static_cast<char*>(malloc(protocoled_data_len));
  if (!protocoled_data) {
    return common::make_error("Can't allocate memory");
  }

  memcpy(protocoled_data, &message_size, sizeof(protocoled_size_t));
  memcpy(protocoled_data + sizeof(protocoled_size_t), data_ptr, data_size);

  *data = protocoled_data;
  *data_len = protocoled_data_len;
  return common::Error();
}
}  // namespace

namespace detail {
common::Error ReadDataSize(common::libev::IoClient* client, protocoled_size_t* sz) {
  if (!client || !sz) {
    return common::make_error_inval();
  }

  protocoled_size_t lsz = 0;
  size_t nread = 0;
  common::Error err = client->Read(reinterpret_cast<char*>(&lsz), sizeof(protocoled_size_t), &nread);
  if (err) {
    return err;
  }

  if (nread != sizeof(protocoled_size_t)) {  // connection closed
    if (nread == 0) {
      return common::make_error("Connection closed");
    }
    return common::make_error(common::MemSPrintf("Error when reading needed to read: %lu bytes, but readed: %lu",
                                                 sizeof(protocoled_size_t), nread));
  }

  *sz = lsz;
  return common::Error();
}

common::Error ReadMessage(common::libev::IoClient* client, char* out, protocoled_size_t size) {
  if (!client || !out || size == 0) {
    return common::make_error_inval();
  }

  size_t nread;
  common::Error err = client->Read(out, size, &nread);
  if (err) {
    return err;
  }

  if (nread != size) {  // connection closed
    if (nread == 0) {
      return common::make_error("Connection closed");
    }
    return common::make_error(
        common::MemSPrintf("Error when reading needed to read: %lu bytes, but readed: %lu", size, nread));
  }

  return common::Error();
}

common::Error ReadCommand(common::libev::IoClient* client, std::string* out) {
  if (!client || !out) {
    return common::make_error_inval();
  }

  protocoled_size_t message_size;
  common::Error err = ReadDataSize(client, &message_size);
  if (err) {
    return err;
  }

  message_size = common::NetToHost32(message_size);  // stable
  if (message_size > MAX_COMMAND_SIZE) {
    return common::make_error(common::MemSPrintf("Reached limit of command size: %u", message_size));
  }

  char* msg = static_cast<char*>(malloc(message_size));
  err = ReadMessage(client, msg, message_size);
  if (err) {
    free(msg);
    return err;
  }

  std::string un_compressed(msg, message_size);
  free(msg);

  *out = un_compressed;
  return common::Error();
}

common::Error WriteMessage(common::libev::IoClient* client, const std::string& message) {
  char* protocoled_data = NULL;
  size_t protocoled_data_len = 0;
  common::Error err = protocol::GenerateProtocoledMessage(message, &protocoled_data, &protocoled_data_len);
  if (err) {
    return err;
  }

  size_t nwrite = 0;
  err = client->Write(protocoled_data, protocoled_data_len, &nwrite);
  free(protocoled_data);
  if (nwrite != protocoled_data_len) {  // connection closed
    return common::make_error(
        common::MemSPrintf("Error when writing needed to write: %lu, but writed: %lu", protocoled_data_len, nwrite));
  }

  return err;
}

common::Error WriteRequest(common::libev::IoClient* client, const request_t& request) {
  return WriteMessage(client, request.GetCmd());
}

common::Error WriteResponce(common::libev::IoClient* client, const responce_t& responce) {
  return WriteMessage(client, responce.GetCmd());
}
}  // namespace detail

}  // namespace protocol
}  // namespace iptv_cloud
