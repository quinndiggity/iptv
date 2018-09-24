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

#include "protocol/protocol.h"

// daemon
// client commands

#define CLIENT_START_STREAM                                                    \
  "start_stream" // {"config": {...}, "command_line": {...} }
#define CLIENT_STOP_STREAM "stop_stream"
#define CLIENT_RESTART_STREAM "restart_stream"

#define CLIENT_ACTIVATE "activate_request" // { "key": "XXXXXXXXXXXXXXXXXX"}
#define CLIENT_STOP_SERVICE "stop_service" // {"delay": 0 }
#define CLIENT_STATE_SERVICE                                                   \
  "state_service" // { "cmd": "save_directory", "jobs_directory": "",
                  // "timeshifts_directory": "", "hls_directory": "",
                  // "ads_directory": "", "playlists_directory": "",
                  // "dvb_directory": "", "capture_card_directory": "" }

#define CLIENT_PING "client_ping"

namespace iptv_cloud {
namespace server {

protocol::responce_t StopServiceResponceSuccess(protocol::sequance_id_t id);
protocol::responce_t StopServiceResponceFail(protocol::sequance_id_t id,
                                             const std::string &error_text);

protocol::request_t StopServiceRequest(protocol::sequance_id_t id,
                                       protocol::serializet_t msg);

protocol::responce_t ActivateResponceSuccess(protocol::sequance_id_t id);

protocol::responce_t StateServiceResponceSuccess(protocol::sequance_id_t id,
                                                 protocol::serializet_t msg);

// streams
protocol::responce_t StartStreamResponceSuccess(protocol::sequance_id_t id);
protocol::responce_t StartStreamResponceFail(protocol::sequance_id_t id,
                                             const std::string &error_text);

protocol::responce_t StopStreamResponceSuccess(protocol::sequance_id_t id);
protocol::responce_t StopStreamResponceFail(protocol::sequance_id_t id,
                                            const std::string &error_text);

protocol::responce_t RestartStreamResponceSuccess(protocol::sequance_id_t id);

protocol::responce_t RestartStreamResponceFail(protocol::sequance_id_t id,
                                               const std::string &error_text);

// requests
// ping
protocol::request_t PingRequest(protocol::sequance_id_t id);
protocol::responce_t PingResponceSuccsess(protocol::sequance_id_t id);
protocol::responce_t PingResponceFail(protocol::sequance_id_t id,
                                      const std::string &error_text); // escaped

} // namespace server
} // namespace iptv_cloud
