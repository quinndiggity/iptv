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

#include "process_wrapper.h"

#include <math.h>

#include <gst/gstcompat.h>

#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>
#include <common/sys_byteorder.h>
#include <common/system_info/system_info.h>
#include <common/time.h>

extern "C" {
#include "sds_fasto.h"  // for sdsfreesplitres, sds
}

#include "stream/ibase_stream.h"

#include "gst_constants.h"
#include "stream/constants.h"  // for ID_FIELD
#include "stream_commands.h"   // for CMD_FIELD

#include "stream_commands_info/changed_sources_info.h"
#include "stream_commands_info/restart_info.h"
#include "stream_commands_info/stop_info.h"
#include "stream_commands_info/stream_struct_info.h"

#include "stream/streams_factory.h"  // for isTimeshiftP...

#include "probes.h"
#include "protocol/protocol.h"

#include "stream/configs_factory.h"

#include "utils/arg_converter.h"

#define DUMP_FILE_NAME "dump.html"

namespace iptv_cloud {
namespace stream {

namespace {

bool PrepareStatus(StreamStruct* stats, StreamStatus st, double cpu_load, std::string* status_out) {
  if (!stats || !status_out) {
    return false;
  }

  if (isnan(cpu_load) || isinf(cpu_load)) {  // stable double
    cpu_load = 0.0;
  }

  long rss = common::system_info::GetProcessRss(static_cast<long>(getpid()));
  const time_t current_time = common::time::current_mstime() / 1000;
  StreamStructInfo sinf(*stats, st, cpu_load, current_time, rss);

  std::string out;
  common::Error err = sinf.SerializeToString(&out);
  if (err) {
    return false;
  }

  *status_out = out;
  return true;
}

class StreamServer : public common::libev::IoLoop {
 public:
  typedef common::libev::IoLoop base_class;
  StreamServer(common::libev::IoClient* command_client, common::libev::IoLoopObserver* observer = nullptr)
      : base_class(new common::libev::LibEvLoop, observer),
        command_client_(static_cast<protocol::protocol_client_t*>(command_client)) {
    CHECK(command_client);
  }

  void WriteRequest(const protocol::request_t& request) WARN_UNUSED_RESULT {
    auto cb = [this, request] { command_client_->WriteRequest(request); };
    ExecInLoopThread(cb);
  }

  virtual const char* ClassName() const override { return "StreamServer"; }

  virtual common::libev::IoChild* CreateChild() override {
    NOTREACHED();
    return nullptr;
  }

  virtual common::libev::IoClient* CreateClient(const common::net::socket_info& info) override {
    UNUSED(info);
    NOTREACHED();
    return nullptr;
  }

  virtual void Started(common::libev::LibEvLoop* loop) override {
    RegisterClient(command_client_);
    base_class::Started(loop);
  }

  virtual void Stopped(common::libev::LibEvLoop* loop) override {
    UnRegisterClient(command_client_);
    base_class::Stopped(loop);
  }

 private:
  protocol::protocol_client_t* const command_client_;
};

}  // namespace

ProcessWrapper::ProcessWrapper(const std::string& process_name,
                               const std::string& feedback_dir,
                               const utils::ArgsMap& config_args,
                               common::libev::IoClient* command_client,
                               StreamStruct* mem)
    : IBaseStream::IStreamClient(),
      max_restart_attempts_(restart_attempts),
      process_name_(process_name),
      feedback_dir_(feedback_dir),
      config_args_(config_args),
      restart_attempts_(0),
      stop_mutex_(),
      stop_cond_(),
      stop_(false),
      channel_id_(),
      ev_thread_(),
      loop_(new StreamServer(command_client, this)),
      ttl_master_timer_(0),
      ttl_sec_(0),
      libev_stated_(2),
      mem_(mem),
      //
      origin_(nullptr),
      id_() {
  CHECK(mem);

  std::string timeshift_dir;
  if (utils::ArgsGetValue(config_args_, TIMESHIFT_DIR_FIELD, &timeshift_dir)) {
    if (!common::file_system::is_directory_exist(timeshift_dir)) {
      common::ErrnoError err = common::file_system::create_directory(timeshift_dir, true);
      if (err) {
        ERROR_LOG() << "Failed to create TimeShift directory, path: " << timeshift_dir;
      } else {
        DEBUG_LOG() << "TimeShift directory exists, path: " << timeshift_dir;
      }
    } else {
      DEBUG_LOG() << "TimeShift directory exists, path: " << timeshift_dir;
    }
  }

  size_t attemps = 0;
  if (utils::ArgsGetValue(config_args_, RESTART_ATTEMPTS_FIELD, &attemps)) {
    max_restart_attempts_ = attemps;
  }

  CHECK(max_restart_attempts_ > 0) << "restart attempts must be grether than 0!";
  if (!utils::ArgsGetValue(config_args_, ID_FIELD, &channel_id_)) {
    CRITICAL_LOG() << "Define " ID_FIELD " variable and make it valid.";
  }

  time_t ttl_sec;
  if (utils::ArgsGetValue(config_args_, AUTO_EXIT_TIME_FIELD, &ttl_sec)) {
    ttl_sec_ = ttl_sec;
  }

  EncoderType enc = CPU;
  std::string video_codec;
  if (utils::ArgsGetValue(config_args, VIDEO_CODEC_FIELD, &video_codec)) {
    EncoderType lenc;
    if (GetTypeEncoder(video_codec, &lenc)) {
      enc = lenc;
    }
  }

  streams_init(0, NULL, enc);
}

ProcessWrapper::~ProcessWrapper() {
  loop_->Stop();
  ev_thread_.join();

  destroy(&loop_);
  streams_deinit();
}

int ProcessWrapper::Exec() {
  ev_thread_ = std::thread([this] {
    int res = loop_->Exec();
    UNUSED(res);
  });
  libev_stated_.Wait();
  TimeShiftInfo tinfo;
  bool is_timeshift_player = IsTimeshiftPlayer(config_args_, &tinfo);
  time_t timeshift_chunk_duration;
  if (is_timeshift_player) {
    if (!utils::ArgsGetValue(config_args_, TIMESHIFT_CHUNK_DURATION_FIELD, &timeshift_chunk_duration)) {
      timeshift_chunk_duration = DEFAULT_TIMESHIFT_CHUNK_DURATION;
    }
  }

  while (!stop_) {
    chunk_index_t start_chunk_index = invalid_chunk_index;
    if (is_timeshift_player) {  // if timeshift player or cathcup player
      while (!tinfo.FindChunkToPlay(timeshift_chunk_duration, &start_chunk_index)) {
        DumpStreamStatus(mem_, SWAITING);

        {
          std::unique_lock<std::mutex> lock(stop_mutex_);
          std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(timeshift_chunk_duration));
          if (interrupt_status == std::cv_status::no_timeout) {  // if notify
            mem_->restarts++;
            break;
          }
        }
      }

      INFO_LOG() << "Founded chunk index " << start_chunk_index;
      if (start_chunk_index == invalid_chunk_index) {
        continue;
      }
    }

    int stabled_status = EXIT_SUCCESS;
    int signal_number = 0;
    time_t start_utc_now = common::time::current_mstime() / 1000;
    origin_ = StreamsFactory::GetInstance().CreateStream(config_args_, this, mem_, start_chunk_index);
    ExitStatus res = origin_->Exec();
    destroy(&origin_);
    if (res == EXIT_INNER) {
      stabled_status = EXIT_FAILURE;
    }

    time_t end_utc_now = common::time::current_mstime() / 1000;
    time_t diff_utc_time = end_utc_now - start_utc_now;
    INFO_LOG() << "Stream exit with status: " << (stabled_status ? "FAILURE" : "SUCCESS")
               << ", signal: " << signal_number << ", working time: " << diff_utc_time << " seconds.";
    if (stabled_status == EXIT_SUCCESS) {
      restart_attempts_ = 0;
      continue;
    }

    if (mem_->WithoutRestartTime() > restart_after_frozen_sec * 10) {  // if longer work
      restart_attempts_ = 0;
      continue;
    }

    size_t wait_time = 0;
    if (++restart_attempts_ == max_restart_attempts_) {
      restart_attempts_ = 0;
      DumpStreamStatus(mem_, SFROZEN);
      wait_time = restart_after_frozen_sec;
    } else {
      wait_time = restart_attempts_ * (restart_after_frozen_sec / max_restart_attempts_);
    }

    INFO_LOG() << process_name_ << " automatically restarted after " << wait_time
               << " seconds, stream restarts: " << mem_->restarts << ", attempts: " << restart_attempts_;

    std::unique_lock<std::mutex> lock(stop_mutex_);
    std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(wait_time));
    if (interrupt_status == std::cv_status::no_timeout) {  // if notify
      restart_attempts_ = 0;
    }
  }

  return EXIT_SUCCESS;
}

void ProcessWrapper::Stop() {
  {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    stop_ = true;
    stop_cond_.notify_all();
  }
  StopStream();
}

void ProcessWrapper::Restart() {
  {
    std::unique_lock<std::mutex> lock(stop_mutex_);
    stop_cond_.notify_all();
  }
  StopStream();
}

void ProcessWrapper::PreLooped(common::libev::IoLoop* loop) {
  UNUSED(loop);

  if (ttl_sec_) {
    ttl_master_timer_ = loop_->CreateTimer(ttl_sec_, false);
    NOTICE_LOG() << "Set stream ttl: " << ttl_sec_;
  }

  libev_stated_.Wait();
  INFO_LOG() << "Child listening started!";
}

void ProcessWrapper::PostLooped(common::libev::IoLoop* loop) {
  UNUSED(loop);
  if (ttl_master_timer_) {
    loop_->RemoveTimer(ttl_master_timer_);
  }
  INFO_LOG() << "Child listening finished!";
}

void ProcessWrapper::Accepted(common::libev::IoClient* client) {
  UNUSED(client);
}

void ProcessWrapper::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
}

void ProcessWrapper::Closed(common::libev::IoClient* client) {
  UNUSED(client);
  Stop();
}

common::ErrnoError ProcessWrapper::StreamDataRecived(common::libev::IoClient* client) {
  std::string input_command;
  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  common::ErrnoError err = pclient->ReadCommand(&input_command);
  if (err) {  // i don't want handle spam, comand must be formated according
              // protocol
    return err;
  }

  common::protocols::three_way_handshake::cmd_id_t seq;
  protocol::sequance_id_t id;
  std::string cmd_str;

  common::Error err_parse = common::protocols::three_way_handshake::ParseCommand(input_command, &seq, &id, &cmd_str);
  if (err_parse) {
    const std::string err_str = err_parse->GetDescription();
    return common::make_errno_error(err_str, EAGAIN);
  }

  int argc;
  sds* argv = sdssplitargslong(cmd_str.c_str(), &argc);
  if (argv == NULL) {
    const std::string error_str = "PROBLEM PARSING INNER COMMAND: " + input_command;
    return common::make_errno_error(error_str, EAGAIN);
  }

  INFO_LOG() << "HANDLE INNER COMMAND client[" << pclient->GetFormatedName()
             << "] seq: " << common::protocols::three_way_handshake::CmdIdToString(seq) << ", id:" << id
             << ", cmd: " << cmd_str;
  if (seq == REQUEST_COMMAND) {
    err = HandleRequestCommand(client, id, argc, argv);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
  } else if (seq == RESPONCE_COMMAND) {
    err = HandleResponceCommand(client, id, argc, argv);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
  } else {
    DNOTREACHED();
    sdsfreesplitres(argv, argc);
    return common::make_errno_error("Invalid command type.", EINVAL);
  }

  sdsfreesplitres(argv, argc);
  return common::ErrnoError();
}

void ProcessWrapper::DataReceived(common::libev::IoClient* client) {
  auto err = StreamDataRecived(client);
  if (err) {
    DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    // client->Close();
    // delete client;
    Stop();
  }
}

void ProcessWrapper::DataReadyToWrite(common::libev::IoClient* client) {
  UNUSED(client);
}

void ProcessWrapper::TimerEmited(common::libev::IoLoop* loop, common::libev::timer_id_t id) {
  UNUSED(loop);
  if (id == ttl_master_timer_) {
    NOTICE_LOG() << "Timeout notified ttl was: " << ttl_sec_;
    Stop();
  }
}

void ProcessWrapper::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void ProcessWrapper::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void ProcessWrapper::ChildStatusChanged(common::libev::IoChild* child, int status) {
  UNUSED(child);
  UNUSED(status);
}

common::ErrnoError ProcessWrapper::HandleRequestCommand(common::libev::IoClient* client,
                                                        protocol::sequance_id_t id,
                                                        int argc,
                                                        char* argv[]) {
  UNUSED(id);
  UNUSED(argc);
  char* command = argv[0];

  if (IS_EQUAL_COMMAND(command, STOP_STREAM)) {
    return HandleRequestStopStream(client, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, RESTART_STREAM)) {
    return HandleRequestRestartStream(client, id, argc, argv);
  } else {
    WARNING_LOG() << "Received unknown command: " << command;
  }

  return common::ErrnoError();
}

common::ErrnoError ProcessWrapper::HandleResponceCommand(common::libev::IoClient* client,
                                                         protocol::sequance_id_t id,
                                                         int argc,
                                                         char* argv[]) {
  UNUSED(client);
  UNUSED(id);
  UNUSED(argc);
  UNUSED(argv);
  return common::ErrnoError();
}

protocol::sequance_id_t ProcessWrapper::NextRequestID() {
  const seq_id_t next_id = id_++;
  char bytes[sizeof(seq_id_t)];
  const seq_id_t stabled = common::NetToHost64(next_id);  // for human readable hex
  memcpy(&bytes, &stabled, sizeof(seq_id_t));
  protocol::sequance_id_t hexed;
  common::utils::hex::encode(std::string(bytes, sizeof(seq_id_t)), true, &hexed);
  return hexed;
}

common::ErrnoError ProcessWrapper::HandleRequestStopStream(common::libev::IoClient* client,
                                                           protocol::sequance_id_t id,
                                                           int argc,
                                                           char* argv[]) {
  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  if (argc > 1) {
    json_object* jstop_info = json_tokener_parse(argv[1]);
    if (!jstop_info) {
      return common::make_errno_error_inval();
    }

    StopInfo stop_info;
    common::Error err_des = stop_info.DeSerialize(jstop_info);
    json_object_put(jstop_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    protocol::responce_t resp = StopStreamResponceSuccess(id);
    pclient->WriteResponce(resp);

    Stop();
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessWrapper::HandleRequestRestartStream(common::libev::IoClient* client,
                                                              protocol::sequance_id_t id,
                                                              int argc,
                                                              char* argv[]) {
  protocol::protocol_client_t* pclient = static_cast<protocol::protocol_client_t*>(client);
  if (argc > 1) {
    json_object* jrestart_info = json_tokener_parse(argv[1]);
    if (!jrestart_info) {
      return common::make_errno_error_inval();
    }

    RestartInfo restart_info;
    common::Error err_des = restart_info.DeSerialize(jrestart_info);
    json_object_put(jrestart_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    protocol::responce_t resp = RestartStreamResponceSuccess(id);
    pclient->WriteResponce(resp);

    Restart();
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

void ProcessWrapper::StopStream() {
  if (origin_) {
    origin_->Quit(EXIT_SELF);
  }
}

void ProcessWrapper::RestartStream() {
  if (origin_) {
    origin_->Restart();
  }
}

void ProcessWrapper::OnStatusChanged(IBaseStream* stream, StreamStatus status) {
  UNUSED(status);
  DumpStreamStatus(stream->GetStats(), stream->GetStatus());
}

GstPadProbeInfo* ProcessWrapper::OnCheckReveivedData(IBaseStream* stream, Probe* probe, GstPadProbeInfo* info) {
  UNUSED(stream);
  UNUSED(probe);
  return info;
}

GstPadProbeInfo* ProcessWrapper::OnCheckReveivedOutputData(IBaseStream* stream, Probe* probe, GstPadProbeInfo* info) {
  UNUSED(stream);
  UNUSED(probe);
  return info;
}

void ProcessWrapper::OnProbeEvent(IBaseStream* stream, Probe* probe, GstEvent* event) {
  UNUSED(stream);
  UNUSED(probe);
  UNUSED(event);
}

void ProcessWrapper::OnPipelineEOS(IBaseStream* stream) {
  stream->Quit(EXIT_INNER);
}

void ProcessWrapper::OnTimeoutUpdated(IBaseStream* stream) {
  DumpStreamStatus(stream->GetStats(), stream->GetStatus());
}

void ProcessWrapper::OnASyncMessageReceived(IBaseStream* stream, GstMessage* message) {
  UNUSED(stream);
  UNUSED(message);
}

void ProcessWrapper::OnSyncMessageReceived(IBaseStream* stream, GstMessage* message) {
  UNUSED(stream);
  UNUSED(message);
}

void ProcessWrapper::OnInputChanged(const InputUri& uri) {
  ChangedSouresInfo ch(uri);
  std::string changed_str;
  common::Error err = ch.SerializeToString(&changed_str);
  if (err) {
    return;
  }

  protocol::request_t req = ChangedSourcesStreamRequest(NextRequestID(), changed_str);
  static_cast<StreamServer*>(loop_)->WriteRequest(req);
}

void ProcessWrapper::OnPipelineCreated(IBaseStream* stream) {
  common::file_system::ascii_directory_string_path feedback_dir(feedback_dir_);
  auto dump_file = feedback_dir.MakeFileStringPath(DUMP_FILE_NAME);
  if (dump_file) {
    stream->DumpIntoFile(*dump_file);
  }
}

void ProcessWrapper::DumpStreamStatus(StreamStruct* stat, StreamStatus st) {
  std::string status;
  if (PrepareStatus(stat, st, common::system_info::GetCpuLoad(getpid()), &status)) {
    protocol::request_t req = StatisticStreamRequest(NextRequestID(), status);
    static_cast<StreamServer*>(loop_)->WriteRequest(req);
  }
}

}  // namespace stream
}  // namespace iptv_cloud
