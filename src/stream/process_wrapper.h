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

#include <common/libev/io_loop.h>
#include <common/libev/io_loop_observer.h>
#include <common/threads/barrier.h>

#include "protocol/types.h"

#include "utils/utils.h"

#include "stream/ibase_stream.h"

namespace iptv_cloud {
namespace stream {

class ProcessWrapper : public common::libev::IoLoopObserver, public IBaseStream::IStreamClient {
 public:
  typedef uint64_t seq_id_t;
  enum constants : uint32_t { restart_attempts = 10, restart_after_frozen_sec = 60 };

  ProcessWrapper(const std::string& process_name,
                 const std::string& feedback_dir,
                 const utils::ArgsMap& config_args,
                 common::libev::IoClient* command_client,
                 StreamStruct* mem);
  ~ProcessWrapper();

  int Exec();

 protected:
  virtual common::Error HandleRequestCommand(common::libev::IoClient* client,
                                             protocol::sequance_id_t id,
                                             int argc,
                                             char* argv[]) WARN_UNUSED_RESULT;
  virtual common::Error HandleResponceCommand(common::libev::IoClient* client,
                                              protocol::sequance_id_t id,
                                              int argc,
                                              char* argv[]) WARN_UNUSED_RESULT;

 private:
  protocol::sequance_id_t NextRequestID();

  common::Error HandleRequestStopStream(common::libev::IoClient* client,
                                        protocol::sequance_id_t id,
                                        int argc,
                                        char* argv[]) WARN_UNUSED_RESULT;
  common::Error HandleRequestRestartStream(common::libev::IoClient* client,
                                           protocol::sequance_id_t id,
                                           int argc,
                                           char* argv[]) WARN_UNUSED_RESULT;

  void Stop();
  void Restart();

  virtual void PreLooped(common::libev::IoLoop* loop) override;
  virtual void PostLooped(common::libev::IoLoop* loop) override;
  virtual void Accepted(common::libev::IoClient* client) override;
  virtual void Moved(common::libev::IoLoop* server, common::libev::IoClient* client) override;
  virtual void Closed(common::libev::IoClient* client) override;

  virtual void TimerEmited(common::libev::IoLoop* loop, common::libev::timer_id_t id) override;

  virtual void Accepted(common::libev::IoChild* child) override;
  virtual void Moved(common::libev::IoLoop* server, common::libev::IoChild* child) override;
  virtual void ChildStatusChanged(common::libev::IoChild* child, int status) override;

  virtual void DataReceived(common::libev::IoClient* client) override;
  virtual void DataReadyToWrite(common::libev::IoClient* client) override;

  common::Error StreamDataRecived(common::libev::IoClient* client) WARN_UNUSED_RESULT;
  void StopStream();
  void RestartStream();

  virtual void OnStatusChanged(IBaseStream* stream, StreamStatus status) override;
  virtual GstPadProbeInfo* OnCheckReveivedData(IBaseStream* stream, Probe* probe, GstPadProbeInfo* info) override;
  virtual GstPadProbeInfo* OnCheckReveivedOutputData(IBaseStream* stream, Probe* probe, GstPadProbeInfo* info) override;
  virtual void OnProbeEvent(IBaseStream* stream, Probe* probe, GstEvent* event) override;
  virtual void OnPipelineEOS(IBaseStream* stream) override;
  virtual void OnTimeoutUpdated(IBaseStream* stream) override;
  virtual void OnSyncMessageReceived(IBaseStream* stream, GstMessage* message) override;
  virtual void OnASyncMessageReceived(IBaseStream* stream, GstMessage* message) override;
  virtual void OnInputChanged(const InputUri& uri) override;

  virtual void OnPipelineCreated(IBaseStream* stream) override;

  common::ErrnoError SendResponceToParent(const std::string& cmd) WARN_UNUSED_RESULT;

  void DumpStreamStatus(StreamStruct* stat, StreamStatus st);

  size_t max_restart_attempts_;
  const std::string process_name_;
  const std::string feedback_dir_;
  const utils::ArgsMap config_args_;
  size_t restart_attempts_;

  std::mutex stop_mutex_;
  std::condition_variable stop_cond_;
  bool stop_;
  std::string channel_id_;

  std::thread ev_thread_;
  common::libev::IoLoop* loop_;
  common::libev::timer_id_t ttl_master_timer_;
  time_t ttl_sec_;
  common::threads::barrier libev_stated_;

  StreamStruct* mem_;

  //
  IBaseStream* origin_;

  std::atomic<seq_id_t> id_;
};

}  // namespace stream
}  // namespace iptv_cloud
