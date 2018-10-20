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

#include "server/process_slave_wrapper.h"

#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/wait.h>

#include <dlfcn.h>
#include <unistd.h>

#include <fstream>

#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>
#include <common/net/net.h>
#include <common/sys_byteorder.h>

extern "C" {
#include "sds_fasto.h"  // for sdsfreesplitres, sds
}

#include "options/options.h"

#include "child_stream.h"
#include "inputs_outputs.h"

#include "stream/constants.h"

#include "stream/main_wrapper.h"

#include "stream_commands.h"

#include "pipe/pipe_client.h"

#include "server/daemon_client.h"
#include "server/daemon_server.h"

#include "server/commands_info/activate_info.h"
#include "server/commands_info/restart_stream_info.h"
#include "server/commands_info/state_service_info.h"
#include "server/commands_info/stop_service_info.h"
#include "server/commands_info/stop_stream_info.h"
#include "server/daemon_commands.h"
#include "server/stream_struct_utils.h"

#include "stream_commands_info/changed_sources_info.h"
#include "stream_commands_info/stream_struct_info.h"

#include "gpu_stats/perf_monitor.h"

#include "utils/arg_converter.h"
#include "utils/utils.h"

#define FIELD_STATS_UPTIME "uptime"
#define FIELD_STATS_TIMESTAM "timestamp"
#define FIELD_STATS_CPU "cpu"
#define FIELD_STATS_GPU "gpu"
#define FIELD_STATS_MEMORY_TOTAL "memory_total"
#define FIELD_STATS_MEMORY_FREE "memory_free"
#define FIELD_STATS_MEMORY_AVAILIBLE "memory_availible"

#define FIELD_STATS_HDD_TOTAL "hdd_total"
#define FIELD_STATS_HDD_FREE "hdd_free"

#define FIELD_STATS_LOAD_AVERAGE "load_average"

#define FIELD_STATS_BANDWIDTH_IN "bandwidth_in"
#define FIELD_STATS_BANDWIDTH_OUT "bandwidth_out"

#define FIELD_STATS_PROJECT_VERSION PROJECT_NAME "_version"

#define DUMMY_LOG_FILE_PATH "/dev/null"

#define DAEMON_STATS_1S "%s:stats"
#define SLAVE_STATS_1S "%s:slave"

#define CLIENT_PORT 6317

#define SAVE_DIRECTORY_FIELD_PATH "path"
#define SAVE_DIRECTORY_FIELD_STATUS "status"
#define SAVE_DIRECTORY_FIELD_RESPONCE "responce"

namespace {

common::ErrnoError create_pipe(int* read_client_fd, int* write_client_fd) {
  if (!read_client_fd || !write_client_fd) {
    return common::make_errno_error_inval();
  }

  int pipefd[2] = {0};
  int res = pipe(pipefd);
  if (res == ERROR_RESULT_VALUE) {
    return common::make_errno_error(errno);
  }

  *read_client_fd = pipefd[0];
  *write_client_fd = pipefd[1];
  return common::ErrnoError();
}

std::string make_daemon_stats_id(std::string id) {
  return common::MemSPrintf(DAEMON_STATS_1S, id);
}

struct DirectoryState {
  DirectoryState(const std::string& dir_str, const char* k) : key(k), dir(), is_valid(false), error_str() {
    if (dir_str.empty()) {
      error_str = "Invalid input.";
      return;
    }

    dir = common::file_system::ascii_directory_string_path(dir_str);
    common::ErrnoError err = common::file_system::node_access(dir.GetPath());
    if (err) {
      error_str = err->GetDescription();
      return;
    }

    is_valid = true;
  }

  std::string key;
  common::file_system::ascii_directory_string_path dir;
  bool is_valid;
  std::string error_str;
};

json_object* MakeDirectoryStateResponce(const DirectoryState& dir) {
  json_object* obj_dir = json_object_new_object();

  json_object* obj = json_object_new_object();
  const std::string path_str = dir.dir.GetPath();
  json_object_object_add(obj, SAVE_DIRECTORY_FIELD_PATH, json_object_new_string(path_str.c_str()));
  json_object_object_add(obj, SAVE_DIRECTORY_FIELD_STATUS, json_object_new_boolean(dir.is_valid));
  json_object_object_add(obj, SAVE_DIRECTORY_FIELD_RESPONCE, json_object_new_string(dir.error_str.c_str()));

  json_object_object_add(obj_dir, dir.key.c_str(), obj);
  return obj_dir;
}

struct Directories {
  explicit Directories(const iptv_cloud::server::StateServiceInfo& sinf)
      : job_dir(sinf.GetJobsDirectory(), STATE_SERVICE_INFO_JOBS_DIRECTORY_FIELD),
        timeshift_dir(sinf.GetTimeshiftsDirectory(), STATE_SERVICE_INFO_TIMESHIFTS_DIRECTORY_FIELD),
        hls_dir(sinf.GetHlsDirectory(), STATE_SERVICE_INFO_HLS_DIRECTORY_FIELD),
        ads_dir(sinf.GetAdsDirectory(), STATE_SERVICE_INFO_ADS_DIRECTORY_FIELD),
        playlist_dir(sinf.GetPlaylistsDirectory(), STATE_SERVICE_INFO_PLAYLIST_DIRECTORY_FIELD),
        dvb_dir(sinf.GetDvbDirectory(), STATE_SERVICE_INFO_DVB_DIRECTORY_FIELD),
        capture_card_dir(sinf.GetCaptureDirectory(), STATE_SERVICE_INFO_CAPTURE_CARD_DIRECTORY_FIELD) {}

  const DirectoryState job_dir;
  const DirectoryState timeshift_dir;
  const DirectoryState hls_dir;
  const DirectoryState ads_dir;
  const DirectoryState playlist_dir;
  const DirectoryState dvb_dir;
  const DirectoryState capture_card_dir;

  bool IsValid() const {
    return job_dir.is_valid && timeshift_dir.is_valid && hls_dir.is_valid && ads_dir.is_valid &&
           playlist_dir.is_valid && dvb_dir.is_valid && capture_card_dir.is_valid;
  }
};

std::string MakeDirectoryResponce(const Directories& dirs) {
  json_object* obj = json_object_new_array();
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.job_dir));
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.timeshift_dir));
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.hls_dir));
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.ads_dir));
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.playlist_dir));
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.dvb_dir));
  json_object_array_add(obj, MakeDirectoryStateResponce(dirs.capture_card_dir));
  std::string obj_str = json_object_get_string(obj);
  json_object_put(obj);
  return obj_str;
}

iptv_cloud::utils::ArgsMap read_slave_config(const std::string& path) {
  if (path.empty()) {
    CRITICAL_LOG() << "Invalid config path!";
  }

  std::ifstream config(path);
  if (!config.is_open()) {
    CRITICAL_LOG() << "Failed to open config path:" << path;
  }

  iptv_cloud::utils::ArgsMap options;
  std::string line;
  while (getline(config, line)) {
    const std::pair<std::string, std::string> pair = iptv_cloud::utils::GetKeyValue(line, '=');

    iptv_cloud::server::options::option_t option;
    if (!iptv_cloud::server::options::FindOption(pair.first, &option)) {
      WARNING_LOG() << "Unknown option: " << pair.first;
    } else {
      switch (option.second(pair.second)) {
        case iptv_cloud::server::options::Validity::VALID:
          options.push_back(pair);
          break;
        case iptv_cloud::server::options::Validity::INVALID:
          WARNING_LOG() << "Invalid value \"" << pair.second << "\" of option " << pair.first;
          break;
        case iptv_cloud::server::options::Validity::FATAL:
          CRITICAL_LOG() << "Invalid value \"" << pair.second << "\" of option " << pair.first;
          break;
        default:
          NOTREACHED();
          break;
      }
    }
  }

  return options;
}

}  // namespace

namespace iptv_cloud {
namespace {
std::string MakeSlaveStatsId(channel_id_t id) {
  return common::MemSPrintf(SLAVE_STATS_1S, id);
}

common::ErrnoError make_stream_info(const utils::ArgsMap& config_args, StreamInfo* sha) {
  if (!sha) {
    return common::make_errno_error_inval();
  }

  StreamInfo lsha;
  if (!utils::ArgsGetValue(config_args, ID_FIELD, &lsha.id)) {
    return common::make_errno_error("Define " ID_FIELD " variable and make it valid.", EAGAIN);
  }

  if (!utils::ArgsGetValue(config_args, TYPE_FIELD, &lsha.type)) {
    return common::make_errno_error("Define " TYPE_FIELD " variable and make it valid.", EAGAIN);
  }

  input_t input;
  if (!read_input(config_args, &input)) {
    return common::make_errno_error("Define " INPUT_FIELD " variable and make it valid.", EAGAIN);
  }

  for (auto input_uri : input) {
    lsha.input.push_back(input_uri.GetID());
  }

  bool is_multi_input = input.size() > 1;
  bool is_timeshift_and_rec =
      (lsha.type == TIMESHIFT_RECORDER && !is_multi_input) || (lsha.type == CATCHUP && !is_multi_input);
  if (is_timeshift_and_rec) {
    std::string timeshift_dir;
    if (utils::ArgsGetValue(config_args, TIMESHIFT_DIR_FIELD, &timeshift_dir)) {
      if (!common::file_system::is_directory_exist(timeshift_dir)) {
        common::ErrnoError errn = common::file_system::create_directory(timeshift_dir, true);
        if (errn) {
          return errn;
        }
      }
    }
    common::ErrnoError errn = common::file_system::node_access(timeshift_dir);
    if (errn) {
      return errn;
    }
  } else {
    output_t output;
    if (!read_output(config_args, &output)) {
      return common::make_errno_error("Define " OUTPUT_FIELD " variable and make it valid.", EAGAIN);
    }

    for (auto out_uri : output) {
      common::uri::Url ouri = out_uri.GetOutput();
      if (ouri.GetScheme() == common::uri::Url::http) {
        const common::file_system::ascii_directory_string_path http_root = out_uri.GetHttpRoot();
        const std::string http_root_str = http_root.GetPath();
        if (!common::file_system::is_directory_exist(http_root_str)) {
          common::ErrnoError errn = common::file_system::create_directory(http_root_str, true);
          if (errn) {
            return errn;
          }
        }
        common::ErrnoError errn = common::file_system::node_access(http_root_str);
        if (errn) {
          return errn;
        }
      }
      lsha.output.push_back(out_uri.GetID());
    }
  }

  *sha = lsha;
  return common::ErrnoError();
}
}  // namespace
namespace server {

struct ProcessSlaveWrapper::NodeStats {
  NodeStats() : prev(), prev_nshot(), gpu_load(0) {}

  utils::CpuShot prev;
  utils::NetShot prev_nshot;
  int gpu_load;
};

ProcessSlaveWrapper::ProcessSlaveWrapper(const std::string& license_key)
    : stats_(nullptr),
      node_id_(),
      start_time_(common::time::current_mstime() / 1000),
      loop_(),
      license_key_(license_key),
      id_(),
      ping_client_id_timer_(INVALID_TIMER_ID),
      node_stats_timer_(INVALID_TIMER_ID),
      cleanup_timer_(INVALID_TIMER_ID),
      node_stats_(new NodeStats) {
  loop_ = new DaemonServer(GetServerHostAndPort(), this);
  loop_->SetName("back_end_server");
  ReadConfig();
}

int ProcessSlaveWrapper::SendStopDaemonRequest(const std::string& license) {
  StopServiceInfo stop_req(license);
  std::string stop_str;
  common::Error serialize_error = stop_req.SerializeToString(&stop_str);
  if (serialize_error) {
    return EXIT_FAILURE;
  }

  protocol::request_t req = StopServiceRequest("0", stop_str);
  common::net::HostAndPort host = GetServerHostAndPort();
  common::net::socket_info client_info;
  common::ErrnoError err = common::net::connect(host, common::net::ST_SOCK_STREAM, 0, &client_info);
  if (err) {
    return EXIT_FAILURE;
  }

  DaemonClient* connection = new DaemonClient(nullptr, client_info);
  static_cast<ProtocoledDaemonClient*>(connection)->WriteRequest(req);
  connection->Close();
  delete connection;
  return EXIT_SUCCESS;
}

ProcessSlaveWrapper::~ProcessSlaveWrapper() {
  ClearStat();
  destroy(&loop_);
  destroy(&node_stats_);
}

int ProcessSlaveWrapper::Exec(int argc, char** argv) {
  process_argc_ = argc;
  process_argv_ = argv;

  // gpu statistic monitor
  std::thread perf_thread;
  gpu_stats::IPerfMonitor* perf_monitor = gpu_stats::CreatePerfMonitor(&node_stats_->gpu_load);
  if (perf_monitor) {
    perf_thread = std::thread([perf_monitor] { perf_monitor->Exec(); });
  }

  int res = EXIT_FAILURE;
  common::libev::tcp::TcpServer* server = static_cast<common::libev::tcp::TcpServer*>(loop_);
  common::ErrnoError err = server->Bind(true);
  if (err) {
    DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    goto finished;
  }

  err = server->Listen(5);
  if (err) {
    DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    goto finished;
  }

  node_stats_->prev = utils::GetMachineCpuShot();
  node_stats_->prev_nshot = utils::GetMachineNetShot();
  res = server->Exec();

finished:
  if (perf_monitor) {
    perf_monitor->Stop();
  }
  if (perf_thread.joinable()) {
    perf_thread.join();
  }
  delete perf_monitor;
  return res;
}

void ProcessSlaveWrapper::ClearStat() {
  if (!stats_) {
    return;
  }

  destroy(&stats_);
}

void ProcessSlaveWrapper::PreLooped(common::libev::IoLoop* server) {
  ping_client_id_timer_ = server->CreateTimer(ping_timeout_clients_seconds, true);
  node_stats_timer_ = server->CreateTimer(node_stats_send_seconds, true);
}

void ProcessSlaveWrapper::Accepted(common::libev::IoClient* client) {
  DaemonClient* dclient = static_cast<DaemonClient*>(client);
  UNUSED(dclient);
}

void ProcessSlaveWrapper::Moved(common::libev::IoLoop* server, common::libev::IoClient* client) {
  UNUSED(server);
  UNUSED(client);
}

void ProcessSlaveWrapper::Closed(common::libev::IoClient* client) {
  UNUSED(client);
}

void ProcessSlaveWrapper::TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) {
  if (ping_client_id_timer_ == id) {
    std::vector<common::libev::IoClient*> online_clients = server->GetClients();
    for (size_t i = 0; i < online_clients.size(); ++i) {
      common::libev::IoClient* client = online_clients[i];
      DaemonClient* dclient = dynamic_cast<DaemonClient*>(client);
      if (dclient && dclient->IsVerified()) {
        ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
        const protocol::request_t ping_request = PingRequest(NextRequestID());
        common::ErrnoError err = pdclient->WriteRequest(ping_request);
        if (err) {
          DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
          err = client->Close();
          DCHECK(!err);
          delete client;
        } else {
          INFO_LOG() << "Pinged to client[" << client->GetFormatedName() << "], from server["
                     << server->GetFormatedName() << "], " << online_clients.size() << " client(s) connected.";
        }
      }
    }
  } else if (node_stats_timer_ == id) {
    if (stats_) {
      utils::CpuShot next = utils::GetMachineCpuShot();
      long double cpu_load = utils::GetCpuMachineLoad(node_stats_->prev, next);
      node_stats_->prev = next;

      utils::NetShot next_nshot = utils::GetMachineNetShot();
      uint64_t bytes_recv = (next_nshot.bytes_recv + node_stats_->prev_nshot.bytes_recv) / 2;
      uint64_t bytes_send = (next_nshot.bytes_send + node_stats_->prev_nshot.bytes_send) / 2;
      node_stats_->prev_nshot = next_nshot;

      utils::MemoryShot mem_shot = utils::GetMachineMemoryShot();
      utils::HddShot hdd_shot = utils::GetMachineHddShot();
      utils::SysinfoShot sshot = utils::GetMachineSysinfoShot();
      std::string uptime_str = common::MemSPrintf("%lu %lu %lu", sshot.loads[0], sshot.loads[1], sshot.loads[2]);
      time_t cur_ts = common::time::current_mstime() / 1000;

      json_object* jstats = json_object_new_object();
      json_object_object_add(jstats, FIELD_STATS_CPU, json_object_new_int(cpu_load * 100));
      json_object_object_add(jstats, FIELD_STATS_GPU, json_object_new_int(node_stats_->gpu_load));
      json_object_object_add(jstats, FIELD_STATS_LOAD_AVERAGE, json_object_new_string(uptime_str.c_str()));
      json_object_object_add(jstats, FIELD_STATS_MEMORY_TOTAL, json_object_new_int64(mem_shot.total_ram));
      json_object_object_add(jstats, FIELD_STATS_MEMORY_FREE, json_object_new_int64(mem_shot.free_ram));
      json_object_object_add(jstats, FIELD_STATS_MEMORY_AVAILIBLE, json_object_new_int64(mem_shot.avail_ram));
      json_object_object_add(jstats, FIELD_STATS_HDD_TOTAL, json_object_new_int64(hdd_shot.hdd_total));
      json_object_object_add(jstats, FIELD_STATS_HDD_FREE, json_object_new_int64(hdd_shot.hdd_free));
      json_object_object_add(jstats, FIELD_STATS_BANDWIDTH_IN, json_object_new_int64(bytes_recv));
      json_object_object_add(jstats, FIELD_STATS_BANDWIDTH_OUT, json_object_new_int64(bytes_send));
      json_object_object_add(jstats, FIELD_STATS_UPTIME, json_object_new_int64(sshot.uptime));
      json_object_object_add(jstats, FIELD_STATS_TIMESTAM, json_object_new_int64(cur_ts));
      json_object_object_add(jstats, FIELD_STATS_PROJECT_VERSION, json_object_new_string(PROJECT_VERSION_HUMAN));
      std::string node_stats = json_object_get_string(jstats);
      json_object_put(jstats);

      bool res = stats_->SetKey(make_daemon_stats_id(node_id_),
                                node_stats);  // node_id_ can be changed in runtime
      if (!res) {
        WARNING_LOG() << "Failed to send statistic: " << node_stats;
      }
    }
  } else if (cleanup_timer_ == id) {
    loop_->Stop();
  }
}

void ProcessSlaveWrapper::Accepted(common::libev::IoChild* child) {
  UNUSED(child);
}

void ProcessSlaveWrapper::Moved(common::libev::IoLoop* server, common::libev::IoChild* child) {
  UNUSED(server);
  UNUSED(child);
}

void ProcessSlaveWrapper::ChildStatusChanged(common::libev::IoChild* child, int status) {
  ChildStream* channel = static_cast<ChildStream*>(child);

  INFO_LOG() << "Successful finished children id: " << channel->GetChannelId();
  int stabled_status = EXIT_SUCCESS;
  int signal_number = 0;

  if (WIFEXITED(status)) {
    stabled_status = WEXITSTATUS(status);
  } else {
    stabled_status = EXIT_FAILURE;
  }
  if (WIFSIGNALED(status)) {
    signal_number = WTERMSIG(status);
  }
  INFO_LOG() << "Stream id: " << channel->GetChannelId()
             << ", exit with status: " << (stabled_status ? "FAILURE" : "SUCCESS") << ", signal: " << signal_number;

  loop_->UnRegisterChild(child);
  delete child;
}

ChildStream* ProcessSlaveWrapper::FindChildByID(const std::string& cid) const {
  auto childs = loop_->GetChilds();
  for (size_t i = 0; i < childs.size(); ++i) {
    ChildStream* lchan = static_cast<ChildStream*>(childs[i]);
    if (lchan->GetChannelId() == cid) {
      return lchan;
    }
  }

  return nullptr;
}

common::ErrnoError ProcessSlaveWrapper::DaemonDataReceived(DaemonClient* dclient) {
  CHECK(loop_->IsLoopThread());
  std::string input_command;
  ProtocoledDaemonClient* pclient = static_cast<ProtocoledDaemonClient*>(dclient);
  common::ErrnoError err = pclient->ReadCommand(&input_command);
  if (err) {
    return err;  // i don't want handle spam, comand must be foramated according
                 // protocol
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
    err = HandleRequestServiceCommand(dclient, id, argc, argv);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
  } else if (seq == RESPONCE_COMMAND) {
    err = HandleResponceServiceCommand(dclient, id, argc, argv);
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

common::ErrnoError ProcessSlaveWrapper::PipeDataReceived(pipe::PipeClient* pipe_client) {
  CHECK(loop_->IsLoopThread());
  std::string input_command;
  pipe::ProtocoledPipeClient* pclient = static_cast<pipe::ProtocoledPipeClient*>(pipe_client);
  common::ErrnoError err = pclient->ReadCommand(&input_command);
  if (err) {
    return err;  // i don't want handle spam, comand must be foramated according
                 // protocol
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
    err = HandleRequestStreamsCommand(pipe_client, id, argc, argv);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
  } else if (seq == RESPONCE_COMMAND) {
    err = HandleResponceStreamsCommand(pipe_client, id, argc, argv);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
    }
  } else {
    NOTREACHED();
  }
  sdsfreesplitres(argv, argc);
  return common::ErrnoError();
}

void ProcessSlaveWrapper::DataReceived(common::libev::IoClient* client) {
  if (DaemonClient* dclient = dynamic_cast<DaemonClient*>(client)) {
    common::ErrnoError err = DaemonDataReceived(dclient);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      dclient->Close();
      delete dclient;
    }
  } else if (pipe::PipeClient* pipe_client = dynamic_cast<pipe::PipeClient*>(client)) {
    common::ErrnoError err = PipeDataReceived(pipe_client);
    if (err) {
      DEBUG_MSG_ERROR(err, common::logging::LOG_LEVEL_ERR);
      auto childs = loop_->GetChilds();
      for (size_t i = 0; i < childs.size(); ++i) {
        ChildStream* lchan = static_cast<ChildStream*>(childs[i]);
        if (pipe_client == lchan->GetPipe()) {
          lchan->SetPipe(nullptr);
          break;
        }
      }

      pipe_client->Close();
      delete pipe_client;
    }
  } else {
    NOTREACHED();
  }
}

void ProcessSlaveWrapper::DataReadyToWrite(common::libev::IoClient* client) {
  DaemonClient* dclient = static_cast<DaemonClient*>(client);
  UNUSED(dclient);
}

void ProcessSlaveWrapper::PostLooped(common::libev::IoLoop* server) {
  if (ping_client_id_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(ping_client_id_timer_);
    ping_client_id_timer_ = INVALID_TIMER_ID;
  }

  if (node_stats_timer_ != INVALID_TIMER_ID) {
    server->RemoveTimer(node_stats_timer_);
    node_stats_timer_ = INVALID_TIMER_ID;
  }
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStopService(DaemonClient* dclient,
                                                                       protocol::sequance_id_t id,
                                                                       int argc,
                                                                       char* argv[]) {
  CHECK(loop_->IsLoopThread());
  if (argc > 1) {
    json_object* jstop = json_tokener_parse(argv[1]);
    if (!jstop) {
      return common::make_errno_error_inval();
    }

    StopServiceInfo stop_info;
    common::Error err_des = stop_info.DeSerialize(jstop);
    json_object_put(jstop);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    bool is_verified_request = stop_info.GetLicense() == license_key_ || dclient->IsVerified();
    if (!is_verified_request) {
      return common::make_errno_error_inval();
    }

    if (cleanup_timer_ != INVALID_TIMER_ID) {
      // in progress
      ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
      protocol::responce_t resp = StopServiceResponceFail(id, "Stop service in progress...");
      pdclient->WriteResponce(resp);

      return common::ErrnoError();
    }

    auto childs = loop_->GetChilds();
    for (size_t i = 0; i < childs.size(); ++i) {
      ChildStream* chan = static_cast<ChildStream*>(childs[i]);
      chan->SendStop(NextRequestID());
    }

    ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
    protocol::responce_t resp = StopServiceResponceSuccess(id);
    pdclient->WriteResponce(resp);

    cleanup_timer_ = loop_->CreateTimer(cleanup_seconds, false);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::CreateChildStream(common::libev::IoLoop* server,
                                                          const StartStreamInfo& start_info) {
  CHECK(loop_->IsLoopThread());
  std::string cmd_argss = start_info.GetCmd();
  std::string config_str = start_info.GetConfig();

  std::vector<std::string> args;
  std::string arg;
  for (size_t i = 0; i != cmd_argss.size(); ++i) {
    char c = cmd_argss[i];
    if (c == ' ') {
      args.push_back(arg);
      arg.clear();
    } else {
      arg += c;
    }
  }
  args.push_back(arg);

  utils::ArgsMap cmd_args = options::ValidateCmdArgs(args);
  utils::ArgsMap config_args = options::ValidateConfig(config_str);
  StreamInfo sha;
  common::ErrnoError err = make_stream_info(config_args, &sha);
  if (err) {
    return err;
  }

  ChildStream* stream = FindChildByID(sha.id);
  if (stream) {
    NOTICE_LOG() << "Skip request to start stream id: " << sha.id;
    return common::make_errno_error(common::MemSPrintf("Stream with id: %s exist, skip request.", sha.id), EINVAL);
  }

  StreamStruct* mem = nullptr;
  err = AllocSharedStreamStruct(sha, &mem);
  if (err) {
    return err;
  }

  int read_command_client = 0;
  int write_requests_client = 0;
  err = create_pipe(&read_command_client, &write_requests_client);
  if (err) {
    FreeSharedStreamStruct(&mem);
    return err;
  }

  int read_responce_client = 0;
  int write_responce_client = 0;
  err = create_pipe(&read_responce_client, &write_responce_client);
  if (err) {
    FreeSharedStreamStruct(&mem);
    return err;
  }

#if !defined(TEST)
  pid_t pid = fork();
#else
  pid_t pid = 0;
#endif
  if (pid == 0) {  // child
    typedef int (*stream_exec_t)(const char* process_name, const struct cmd_args* cmd_args, void* config_args,
                                 void* command_client, void* mem);
    const std::string absolute_source_dir = common::file_system::absolute_path_from_relative(RELATIVE_SOURCE_DIR);
    const std::string lib_full_path = common::file_system::make_path(absolute_source_dir, CORE_LIBRARY);
    void* handle = dlopen(lib_full_path.c_str(), RTLD_LAZY);
    if (!handle) {
      ERROR_LOG() << "Failed to load " CORE_LIBRARY " path: " << lib_full_path
                  << ", error: " << common::common_strerror(errno);
      _exit(EXIT_FAILURE);
    }

    char* error;
    stream_exec_t stream_exec_func = reinterpret_cast<stream_exec_t>(dlsym(handle, "stream_exec"));
    if ((error = dlerror()) != NULL) {
      ERROR_LOG() << "Failed to load start stream function error: " << error;
      dlclose(handle);
      _exit(EXIT_FAILURE);
    }

    int logs_level;
    if (!utils::ArgsGetValue(cmd_args, LOG_LEVEL_FIELD, &logs_level)) {
      logs_level = common::logging::LOG_LEVEL_DEBUG;
    }

    std::string feedback_dir;
    if (!utils::ArgsGetValue(cmd_args, FEEDBACK_DIR_FIELD, &feedback_dir)) {
      ERROR_LOG() << "Define " FEEDBACK_DIR_FIELD " variable and make it valid.";
      dlclose(handle);
      _exit(EXIT_FAILURE);
    }

    const struct cmd_args client_args = {feedback_dir.c_str(), logs_level};
    const std::string new_process_name = common::MemSPrintf(STREAMER_NAME " %s", sha.id);
    for (int i = 0; i < process_argc_; ++i) {
      memset(process_argv_[i], 0, strlen(process_argv_[i]));
    }
    const char* new_name = new_process_name.c_str();
    char* app_name = process_argv_[0];
    strncpy(app_name, new_name, new_process_name.length());
    app_name[new_process_name.length()] = 0;
    prctl(PR_SET_NAME, new_name);

#if !defined(TEST)
    // close not needed pipes
    common::ErrnoError errn = common::file_system::close_descriptor(read_responce_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    errn = common::file_system::close_descriptor(write_requests_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
#endif

    pipe::PipeClient* client = new pipe::PipeClient(nullptr, read_command_client, write_responce_client);
    int res = stream_exec_func(new_name, &client_args, &config_args, client, mem);
    dlclose(handle);
    client->Close();
    delete client;
    _exit(res);
  } else if (pid < 0) {
    NOTICE_LOG() << "Failed to start children!";
  } else {
    // close not needed pipes
    common::ErrnoError errn = common::file_system::close_descriptor(read_command_client);
    if (errn) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }
    errn = common::file_system::close_descriptor(write_responce_client);
    if (err) {
      DEBUG_MSG_ERROR(errn, common::logging::LOG_LEVEL_WARNING);
    }

    pipe::PipeClient* pipe_client = new pipe::PipeClient(server, read_responce_client, write_requests_client);
    server->RegisterClient(pipe_client);
    ChildStream* new_channel = new ChildStream(loop_, mem);
    new_channel->SetPipe(pipe_client);
    loop_->RegisterChild(new_channel, pid);
  }

  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestChangedSourcesStream(pipe::PipeClient* pclient,
                                                                          protocol::sequance_id_t id,
                                                                          int argc,
                                                                          char* argv[]) {
  CHECK(loop_->IsLoopThread());
  if (argc > 1) {
    json_object* jrequest_changed_sources = json_tokener_parse(argv[1]);
    if (!jrequest_changed_sources) {
      return common::make_errno_error_inval();
    }

    ChangedSouresInfo ch_sources_info;
    common::Error err_des = ch_sources_info.DeSerialize(jrequest_changed_sources);
    json_object_put(jrequest_changed_sources);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    pipe::ProtocoledPipeClient* pdclient = static_cast<pipe::ProtocoledPipeClient*>(pclient);
    protocol::responce_t resp = ChangedSourcesStreamResponceSuccess(id);
    pdclient->WriteResponce(resp);

    std::string changed_str;
    common::Error err_ser = ch_sources_info.SerializeToString(&changed_str);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    // notify subscribers
    std::vector<common::libev::IoClient*> clients = loop_->GetClients();
    for (size_t i = 0; i < clients.size(); ++i) {
      DaemonClient* dclient = dynamic_cast<DaemonClient*>(clients[i]);
      if (dclient) {
        protocol::request_t req = ChangedSourcesStreamRequest(NextRequestID(), changed_str);
        ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
        pdclient->WriteRequest(req);
      }
    }
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestStatisticStream(pipe::PipeClient* pclient,
                                                                     protocol::sequance_id_t id,
                                                                     int argc,
                                                                     char* argv[]) {
  CHECK(loop_->IsLoopThread());
  if (argc > 1) {
    json_object* jrequest_stat = json_tokener_parse(argv[1]);
    if (!jrequest_stat) {
      return common::make_errno_error_inval();
    }

    StreamStructInfo stat;
    common::Error err_des = stat.DeSerialize(jrequest_stat);
    json_object_put(jrequest_stat);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    std::string status;
    common::Error err_ser = stat.SerializeToString(&status);
    if (err_ser) {
      const std::string err_str = err_ser->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    pipe::ProtocoledPipeClient* pdclient = static_cast<pipe::ProtocoledPipeClient*>(pclient);
    protocol::responce_t resp = StatisticStreamResponceSuccess(id);
    pdclient->WriteResponce(resp);

    auto struc = stat.GetStreamStruct();
    stats_->SetKey(MakeSlaveStatsId(struc->id), status);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStartStream(DaemonClient* dclient,
                                                                       protocol::sequance_id_t id,
                                                                       int argc,
                                                                       char* argv[]) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (argc > 1) {
    json_object* jstart_info = json_tokener_parse(argv[1]);
    if (!jstart_info) {
      return common::make_errno_error_inval();
    }

    StartStreamInfo start_info;
    common::Error err_des = start_info.DeSerialize(jstart_info);
    json_object_put(jstart_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
    common::libev::IoLoop* server = dclient->GetServer();
    common::ErrnoError err = CreateChildStream(server, start_info);
    if (err) {
      protocol::responce_t resp = StartStreamResponceFail(id, err->GetDescription());
      pdclient->WriteResponce(resp);
      return err;
    }

    protocol::responce_t resp = StartStreamResponceSuccess(id);
    pdclient->WriteResponce(resp);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

protocol::sequance_id_t ProcessSlaveWrapper::NextRequestID() {
  const seq_id_t next_id = id_++;
  char bytes[sizeof(seq_id_t)];
  const seq_id_t stabled = common::NetToHost64(next_id);  // for human readable hex
  memcpy(&bytes, &stabled, sizeof(seq_id_t));
  protocol::sequance_id_t hexed;
  common::utils::hex::encode(std::string(bytes, sizeof(seq_id_t)), true, &hexed);
  return hexed;
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStopStream(DaemonClient* dclient,
                                                                      protocol::sequance_id_t id,
                                                                      int argc,
                                                                      char* argv[]) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (argc > 1) {
    json_object* jstop_info = json_tokener_parse(argv[1]);
    if (!jstop_info) {
      return common::make_errno_error_inval();
    }

    StopStreamInfo stop_info;
    common::Error err_des = stop_info.DeSerialize(jstop_info);
    json_object_put(jstop_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    ChildStream* chan = FindChildByID(stop_info.GetStreamID());
    ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
    if (!chan) {
      protocol::responce_t resp = StopStreamResponceFail(id, "Stream not found.");
      pdclient->WriteResponce(resp);
      return common::ErrnoError();
    }

    chan->SendStop(NextRequestID());
    protocol::responce_t resp = StopStreamResponceSuccess(id);
    pdclient->WriteResponce(resp);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientRestartStream(DaemonClient* dclient,
                                                                         protocol::sequance_id_t id,
                                                                         int argc,
                                                                         char* argv[]) {
  CHECK(loop_->IsLoopThread());
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (argc > 1) {
    json_object* jrestart_info = json_tokener_parse(argv[1]);
    if (!jrestart_info) {
      return common::make_errno_error_inval();
    }

    RestartStreamInfo restart_info;
    common::Error err_des = restart_info.DeSerialize(jrestart_info);
    json_object_put(jrestart_info);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    ChildStream* chan = FindChildByID(restart_info.GetStreamID());
    ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
    if (!chan) {
      protocol::responce_t resp = RestartStreamResponceFail(id, "Stream not found.");
      pdclient->WriteResponce(resp);
      return common::ErrnoError();
    }

    chan->SendRestart(NextRequestID());
    protocol::responce_t resp = RestartStreamResponceSuccess(id);
    pdclient->WriteResponce(resp);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientStateService(DaemonClient* dclient,
                                                                        protocol::sequance_id_t id,
                                                                        int argc,
                                                                        char* argv[]) {
  if (!dclient->IsVerified()) {
    return common::make_errno_error_inval();
  }

  if (argc > 1) {
    json_object* jservice_state = json_tokener_parse(argv[1]);
    if (!jservice_state) {
      return common::make_errno_error_inval();
    }

    StateServiceInfo state_info;
    common::Error err_des = state_info.DeSerialize(jservice_state);
    json_object_put(jservice_state);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
    Directories dirs(state_info);
    std::string resp_str = MakeDirectoryResponce(dirs);
    protocol::responce_t resp = StateServiceResponceSuccess(id, resp_str);
    pdclient->WriteResponce(resp);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestClientActivate(DaemonClient* dclient,
                                                                    protocol::sequance_id_t id,
                                                                    int argc,
                                                                    char* argv[]) {
  if (argc > 1) {
    json_object* jactivate = json_tokener_parse(argv[1]);
    if (!jactivate) {
      return common::make_errno_error_inval();
    }

    ActivateInfo activate_info;
    common::Error err_des = activate_info.DeSerialize(jactivate);
    json_object_put(jactivate);
    if (err_des) {
      const std::string err_str = err_des->GetDescription();
      return common::make_errno_error(err_str, EAGAIN);
    }

    bool is_active = activate_info.GetLicense() == license_key_;
    if (!is_active) {
      return common::make_errno_error_inval();
    }

    ProtocoledDaemonClient* pdclient = static_cast<ProtocoledDaemonClient*>(dclient);
    protocol::responce_t resp = ActivateResponceSuccess(id);
    pdclient->WriteResponce(resp);
    dclient->SetVerified(true);
    return common::ErrnoError();
  }

  return common::make_errno_error_inval();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestServiceCommand(DaemonClient* dclient,
                                                                    protocol::sequance_id_t id,
                                                                    int argc,
                                                                    char* argv[]) {
  UNUSED(id);
  UNUSED(argc);
  char* command = argv[0];

  if (IS_EQUAL_COMMAND(command, CLIENT_START_STREAM)) {
    return HandleRequestClientStartStream(dclient, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, CLIENT_STOP_STREAM)) {
    return HandleRequestClientStopStream(dclient, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, CLIENT_RESTART_STREAM)) {
    return HandleRequestClientRestartStream(dclient, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, CLIENT_STATE_SERVICE)) {
    return HandleRequestClientStateService(dclient, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, CLIENT_STOP_SERVICE)) {
    return HandleRequestClientStopService(dclient, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, CLIENT_ACTIVATE)) {
    return HandleRequestClientActivate(dclient, id, argc, argv);
  } else {
    WARNING_LOG() << "Received unknown command: " << command;
  }

  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleResponceServiceCommand(DaemonClient* dclient,
                                                                     protocol::sequance_id_t id,
                                                                     int argc,
                                                                     char* argv[]) {
  UNUSED(dclient);
  UNUSED(id);
  UNUSED(argc);
  UNUSED(argv);
  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleRequestStreamsCommand(pipe::PipeClient* pclient,
                                                                    protocol::sequance_id_t id,
                                                                    int argc,
                                                                    char* argv[]) {
  UNUSED(id);
  UNUSED(argc);
  char* command = argv[0];

  if (IS_EQUAL_COMMAND(command, CHANGED_SOURCES_STREAM)) {
    return HandleRequestChangedSourcesStream(pclient, id, argc, argv);
  } else if (IS_EQUAL_COMMAND(command, STATISTIC_STREAM)) {
    return HandleRequestStatisticStream(pclient, id, argc, argv);
  } else {
    WARNING_LOG() << "Received unknown command: " << command;
  }
  return common::ErrnoError();
}

common::ErrnoError ProcessSlaveWrapper::HandleResponceStreamsCommand(pipe::PipeClient* pclient,
                                                                     protocol::sequance_id_t id,
                                                                     int argc,
                                                                     char* argv[]) {
  UNUSED(pclient);
  UNUSED(id);
  UNUSED(argc);
  UNUSED(argv);
  return common::ErrnoError();
}

void ProcessSlaveWrapper::ReadConfig() {  // CONFIG_SLAVE_FILE_PATH
  utils::ArgsMap slave_config_args = read_slave_config(CONFIG_SLAVE_FILE_PATH);
  if (!utils::ArgsGetValue(slave_config_args, ID_FIELD, &node_id_)) {
    CRITICAL_LOG() << "Define " ID_FIELD " variable and make it valid.";
  }

  std::string stats_data;
  if (!utils::ArgsGetValue(slave_config_args, STATS_CREDENTIALS_FIELD, &stats_data)) {
    CRITICAL_LOG() << "Define " STATS_CREDENTIALS_FIELD " variable and make it valid.";
  }

  if (!utils::ArgsGetValue(slave_config_args, LOG_FILE_FIELD, &log_path_)) {
    WARNING_LOG() << "Define " LOG_FILE_FIELD " variable and make it valid, now used: " DUMMY_LOG_FILE_PATH ".";
    log_path_ = DUMMY_LOG_FILE_PATH;
  }

  stats::StatCredentialsBase* screds = stats::StatCredentialsBase::CreateCreadentialsFromString(stats_data);
  CHECK(screds);
  ClearStat();
  stats_ = stats::IStat::CreateStat(screds);
}

common::net::HostAndPort ProcessSlaveWrapper::GetServerHostAndPort() {
  return common::net::HostAndPort::CreateLocalHost(CLIENT_PORT);
}

std::string ProcessSlaveWrapper::GetLogPath() const {
  return log_path_;
}

}  // namespace server
}  // namespace iptv_cloud
