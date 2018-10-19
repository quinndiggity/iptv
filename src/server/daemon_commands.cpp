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

#include "server/daemon_commands.h"

// activate
#define CLIENT_ACTIVATE_RESP_FAIL_1E GENEATATE_FAIL_FMT(CLIENT_ACTIVATE, "'%s'")
#define CLIENT_ACTIVATE_RESP_SUCCESS GENEATATE_SUCCESS(CLIENT_ACTIVATE)

// start stream
#define CLIENT_START_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(CLIENT_START_STREAM)
#define CLIENT_START_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(CLIENT_START_STREAM, "'%s'")
// stop stream
#define CLIENT_STOP_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(CLIENT_STOP_STREAM)
#define CLIENT_STOP_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(CLIENT_STOP_STREAM, "'%s'")
// stop restart
#define CLIENT_RESTART_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(CLIENT_RESTART_STREAM)
#define CLIENT_RESTART_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(CLIENT_RESTART_STREAM, "'%s'")

// stop service
#define CLIENT_STOP_SERVICE_REQ_1E GENERATE_REQUEST_FMT_ARGS(CLIENT_STOP_SERVICE, "'%s'")
#define CLIENT_STOP_SERVICE_RESP_FAIL_1E GENEATATE_FAIL_FMT(CLIENT_STOP_SERVICE, "'%s'")
#define CLIENT_STOP_SERVICE_RESP_SUCCESS GENEATATE_SUCCESS(CLIENT_STOP_SERVICE)

// service state
#define CLIENT_STATE_SERVICE_RESP_SUCCESS_1E GENEATATE_SUCCESS_FMT(CLIENT_STATE_SERVICE, "'%s'")

// requests
// ping
#define CLIENT_PING_REQ GENERATE_REQUEST_FMT(CLIENT_PING)
#define CLIENT_PING_RESP_FAIL_1E GENEATATE_FAIL_FMT(CLIENT_PING, "'%s'")
#define CLIENT_PING_RESP_SUCCESS GENEATATE_SUCCESS(CLIENT_PING)

namespace iptv_cloud {
namespace server {

protocol::responce_t StopServiceResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_STOP_SERVICE_RESP_SUCCESS);
}

protocol::responce_t StopServiceResponceFail(protocol::sequance_id_t id, const std::string& error_text) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_STOP_SERVICE_RESP_FAIL_1E, error_text);
}

protocol::request_t StopServiceRequest(protocol::sequance_id_t id, protocol::serializet_t msg) {
  return common::protocols::three_way_handshake::MakeRequest(id, CLIENT_STOP_SERVICE_REQ_1E, msg);
}

protocol::responce_t ActivateResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_ACTIVATE_RESP_SUCCESS);
}

protocol::responce_t StateServiceResponceSuccess(protocol::sequance_id_t id, protocol::serializet_t msg) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_STATE_SERVICE_RESP_SUCCESS_1E, msg);
}

protocol::responce_t StartStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_START_STREAM_RESP_SUCCESS);
}

protocol::responce_t StartStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_START_STREAM_RESP_FAIL_1E, error_text);
}

protocol::responce_t StopStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_STOP_STREAM_RESP_SUCCESS);
}

protocol::responce_t StopStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_STOP_STREAM_RESP_FAIL_1E, error_text);
}

protocol::responce_t RestartStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_RESTART_STREAM_RESP_SUCCESS);
}

protocol::responce_t RestartStreamResponceFail(protocol::sequance_id_t id, const std::string& error_text) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_RESTART_STREAM_RESP_FAIL_1E, error_text);
}

protocol::request_t PingRequest(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeRequest(id, CLIENT_PING_REQ);
}

protocol::responce_t PingResponceSuccsess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_PING_RESP_SUCCESS);
}

protocol::responce_t PingResponceFail(protocol::sequance_id_t id, const std::string& error_text) {
  return common::protocols::three_way_handshake::MakeResponce(id, CLIENT_PING_RESP_FAIL_1E, error_text);
}

}  // namespace server
}  // namespace iptv_cloud
