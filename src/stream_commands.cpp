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

#include "stream_commands.h"

#define RESTART_STREAM_REQ_1E GENERATE_REQUEST_FMT_ARGS(RESTART_STREAM, "'%s'")
#define RESTART_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(RESTART_STREAM)
#define RESTART_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(RESTART_STREAM, "'%s'")

#define STOP_STREAM_REQ_1E GENERATE_REQUEST_FMT_ARGS(STOP_STREAM, "'%s'")
#define STOP_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(STOP_STREAM)
#define STOP_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(STOP_STREAM, "'%s'")

#define CHANGED_SOURCES_STREAM_REQ_1E GENERATE_REQUEST_FMT_ARGS(CHANGED_SOURCES_STREAM, "'%s'")
#define CHANGED_SOURCES_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(CHANGED_SOURCES_STREAM)
#define CHANGED_SOURCES_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(CHANGED_SOURCES_STREAM, "'%s'")

#define STATISTIC_STREAM_REQ_1E GENERATE_REQUEST_FMT_ARGS(STATISTIC_STREAM, "'%s'")
#define STATISTIC_STREAM_RESP_SUCCESS GENEATATE_SUCCESS(STATISTIC_STREAM)
#define STATISTIC_STREAM_RESP_FAIL_1E GENEATATE_FAIL_FMT(STATISTIC_STREAM, "'%s'")

namespace iptv_cloud {

protocol::request_t RestartStreamRequest(protocol::sequance_id_t id, protocol::serializet_t msg) {
  return common::protocols::three_way_handshake::MakeRequest(id, RESTART_STREAM_REQ_1E, msg);
}

protocol::responce_t RestartStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, RESTART_STREAM_RESP_SUCCESS);
}

protocol::request_t StopStreamRequest(protocol::sequance_id_t id, protocol::serializet_t msg) {
  return common::protocols::three_way_handshake::MakeRequest(id, STOP_STREAM_REQ_1E, msg);
}

protocol::responce_t StopStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, STOP_STREAM_RESP_SUCCESS);
}

protocol::request_t ChangedSourcesStreamRequest(protocol::sequance_id_t id, protocol::serializet_t msg) {
  return common::protocols::three_way_handshake::MakeRequest(id, CHANGED_SOURCES_STREAM_REQ_1E, msg);
}

protocol::responce_t ChangedSourcesStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, CHANGED_SOURCES_STREAM_RESP_SUCCESS);
}

protocol::request_t StatisticStreamRequest(protocol::sequance_id_t id, protocol::serializet_t msg) {
  return common::protocols::three_way_handshake::MakeRequest(id, STATISTIC_STREAM_REQ_1E, msg);
}

protocol::responce_t StatisticStreamResponceSuccess(protocol::sequance_id_t id) {
  return common::protocols::three_way_handshake::MakeResponce(id, STATISTIC_STREAM_RESP_SUCCESS);
}

}  // namespace iptv_cloud
