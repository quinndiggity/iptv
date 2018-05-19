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

#include <thread>

#include <common/libev/io_loop_observer.h>
#include <common/net/types.h>
#include <common/protocols/three_way_handshake/commands.h>

#include "stats/istat.h"

#include "protocol/types.h"

#include "server/commands_info/start_stream_info.h"

namespace iptv_cloud {
namespace server {
class ChildStream;
namespace pipe {
class PipeClient;
}
class DaemonClient;

class ProcessSlaveWrapper : public common::libev::IoLoopObserver {
 public:
  typedef uint64_t seq_id_t;
  enum { node_stats_send_seconds = 10, ping_timeout_clients_seconds = 60, cleanup_seconds = 5 };
  ProcessSlaveWrapper(const std::string& licensy_key);
  virtual ~ProcessSlaveWrapper();

  static int SendStopDaemonRequest(const std::string& license);

  int Exec(int argc, char** argv);

  std::string GetLogPath() const;

 protected:
  virtual void PreLooped(common::libev::IoLoop* server) override;
  virtual void Accepted(common::libev::IoClient* client) override;
  virtual void Moved(common::libev::IoLoop* server,
                     common::libev::IoClient* client) override;  // owner server, now client is orphan
  virtual void Closed(common::libev::IoClient* client) override;
  virtual void TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) override;

  virtual void Accepted(common::libev::IoChild* child) override;
  virtual void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  virtual void ChildStatusChanged(common::libev::IoChild* child, int status) override;

  virtual void DataReceived(common::libev::IoClient* client) override;
  virtual void DataReadyToWrite(common::libev::IoClient* client) override;
  virtual void PostLooped(common::libev::IoLoop* server) override;

  virtual common::Error HandleRequestServiceCommand(DaemonClient* dclient,
                                                    protocol::sequance_id_t id,
                                                    int argc,
                                                    char* argv[]) WARN_UNUSED_RESULT;
  virtual common::Error HandleResponceServiceCommand(DaemonClient* dclient,
                                                     protocol::sequance_id_t id,
                                                     int argc,
                                                     char* argv[]) WARN_UNUSED_RESULT;

  virtual common::Error HandleRequestStreamsCommand(pipe::PipeClient* pclient,
                                                    protocol::sequance_id_t id,
                                                    int argc,
                                                    char* argv[]) WARN_UNUSED_RESULT;
  virtual common::Error HandleResponceStreamsCommand(pipe::PipeClient* pclient,
                                                     protocol::sequance_id_t id,
                                                     int argc,
                                                     char* argv[]) WARN_UNUSED_RESULT;

 private:
  ChildStream* FindChildByID(const std::string& cid) const;

  struct NodeStats;
  common::Error DaemonDataReceived(DaemonClient* dclient) WARN_UNUSED_RESULT;
  common::Error PipeDataReceived(pipe::PipeClient* pclient) WARN_UNUSED_RESULT;

  protocol::sequance_id_t NextRequestID();

  common::Error CreateChildStream(common::libev::IoLoop* server, const StartStreamInfo& start_info);

  common::Error HandleRequestChangedSourcesStream(pipe::PipeClient* pclient,
                                                  protocol::sequance_id_t id,
                                                  int argc,
                                                  char* argv[]) WARN_UNUSED_RESULT;

  common::Error HandleRequestStatisticStream(pipe::PipeClient* pclient,
                                             protocol::sequance_id_t id,
                                             int argc,
                                             char* argv[]) WARN_UNUSED_RESULT;

  common::Error HandleRequestClientStartStream(DaemonClient* dclient,
                                               protocol::sequance_id_t id,
                                               int argc,
                                               char* argv[]) WARN_UNUSED_RESULT;
  common::Error HandleRequestClientStopStream(DaemonClient* dclient, protocol::sequance_id_t id, int argc, char* argv[])
      WARN_UNUSED_RESULT;
  common::Error HandleRequestClientRestartStream(DaemonClient* dclient,
                                                 protocol::sequance_id_t id,
                                                 int argc,
                                                 char* argv[]) WARN_UNUSED_RESULT;

  // service
  common::Error HandleRequestClientStateService(DaemonClient* dclient,
                                                protocol::sequance_id_t id,
                                                int argc,
                                                char* argv[]) WARN_UNUSED_RESULT;
  common::Error HandleRequestClientActivate(DaemonClient* dclient, protocol::sequance_id_t id, int argc, char* argv[])
      WARN_UNUSED_RESULT;
  common::Error HandleRequestClientStopService(DaemonClient* dclient,
                                               protocol::sequance_id_t id,
                                               int argc,
                                               char* argv[]) WARN_UNUSED_RESULT;

  void ReadConfig();
  void ClearStat();

  static common::net::HostAndPort GetServerHostAndPort();

  stats::IStat* stats_;  // all calls should be in loop thread

  std::string node_id_;
  const time_t start_time_;

  int process_argc_;
  char** process_argv_;

  std::string log_path_;

  common::libev::IoLoop* loop_;

  const std::string license_key_;
  std::atomic<seq_id_t> id_;
  common::libev::timer_id_t ping_client_id_timer_;
  common::libev::timer_id_t node_stats_timer_;
  common::libev::timer_id_t cleanup_timer_;
  NodeStats* node_stats_;
};

}  // namespace server
}  // namespace iptv_cloud
