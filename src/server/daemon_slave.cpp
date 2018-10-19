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

// gst-launch-1.0 uridecodebin
// uri=http://sak101.fastbroad.com/i/klan109_1@356154/master.m3u8
// caps="video/x-h264,stream-format=byte-stream" ! h264parse ! flvmux ! rtmpsink
// location=rtmp://4.31.30.153:1935/devapp/fashion_one
// some links:
// http://gstreamer-devel.narkive.com/YF3JF3qj/issue-in-seeking-while-streaming-video-files

#include <string.h>

#include <iostream>

#include <fcntl.h>

#include <sys/stat.h>
#include <unistd.h>

#include <common/file_system/file.h>
#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>
#include <common/utils.h>

#include "server/process_slave_wrapper.h"

#include "license/gen_hardware_hash.h"

#define HELP_TEXT                          \
  "Usage: " STREAMER_SERVICE_NAME          \
  " [options]\n"                           \
  "  Manipulate " STREAMER_SERVICE_NAME    \
  ".\n\n"                                  \
  "    --version  display version\n"       \
  "    --daemon   run as a daemon\n"       \
  "    --stop     stop running instance\n" \
  "    --reload   force running instance to reread configuration file\n"

namespace {

const int default_log_file_size_kb = 1024;

bool create_license_key(std::string* license_key) {
#if HARDWARE_LICENSE_ALGO == 0
  static const iptv_cloud::server::license::ALGO_TYPE license_algo = iptv_cloud::server::license::HDD;
#elif HARDWARE_LICENSE_ALGO == 1
  static const iptv_cloud::server::license::ALGO_TYPE license_algo = iptv_cloud::server::license::MACHINE_ID;
#else
#error Unknown hardware license algo used
#endif

  if (!license_key) {
    return false;
  }

  if (SIZEOFMASS(LICENSE_KEY) == 1) {  // runtime check
    CRITICAL_LOG() << "A-a-a license key is empty, don't hack me!";
  }

  if (!iptv_cloud::server::license::GenerateHardwareHash(license_algo, license_key)) {
    WARNING_LOG() << "Failed to generate hash!";
    return false;
  }

  if (*license_key != LICENSE_KEY) {
    ERROR_LOG() << "License keys not same!";
    return false;
  }
  return true;
}
}  // namespace

int main(int argc, char** argv, char** envp) {
  bool run_as_daemon = false;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--version") == 0) {
      std::cout << PROJECT_VERSION_HUMAN << std::endl;
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "--daemon") == 0) {
      run_as_daemon = true;
    } else if (strcmp(argv[i], "--stop") == 0) {
      std::string license_key;
      if (!create_license_key(&license_key)) {
        return EXIT_FAILURE;
      }

      return iptv_cloud::server::ProcessSlaveWrapper::SendStopDaemonRequest(license_key);
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      std::cout << HELP_TEXT << std::endl;
      return EXIT_SUCCESS;
    } else {
      std::cout << HELP_TEXT << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (run_as_daemon) {
    if (!common::create_as_daemon()) {
      return EXIT_FAILURE;
    }
  }

  pid_t daemon_pid = getpid();
  std::string folder_path_to_pid = common::file_system::get_dir_path(PIDFILE_PATH);
  if (folder_path_to_pid.empty()) {
    ERROR_LOG() << "Can't get pid file path: " << PIDFILE_PATH;
    return EXIT_FAILURE;
  }

  if (!common::file_system::is_directory_exist(folder_path_to_pid)) {
    if (!common::file_system::create_directory(folder_path_to_pid, true)) {
      ERROR_LOG() << "Pid file directory not exists, pid file path: " << PIDFILE_PATH;
      return EXIT_FAILURE;
    }
  }

  common::ErrnoError err = common::file_system::node_access(folder_path_to_pid);
  if (err) {
    ERROR_LOG() << "Can't have permissions to create, pid file path: " << PIDFILE_PATH;
    return EXIT_FAILURE;
  }

  common::file_system::File pidfile;
  err = pidfile.Open(PIDFILE_PATH, common::file_system::File::FLAG_CREATE | common::file_system::File::FLAG_WRITE);
  if (err) {
    ERROR_LOG() << "Can't open pid file path: " << PIDFILE_PATH;
    return EXIT_FAILURE;
  }

  err = pidfile.Lock();
  if (err) {
    ERROR_LOG() << "Can't lock pid file path: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }
  std::string pid_str = common::MemSPrintf("%ld\n", static_cast<long>(daemon_pid));
  size_t writed;
  err = pidfile.Write(pid_str, &writed);
  if (err) {
    ERROR_LOG() << "Failed to write pid file path: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  std::string license_key;
  if (!create_license_key(&license_key)) {
    return EXIT_FAILURE;
  }

  // start
  iptv_cloud::server::ProcessSlaveWrapper wrapper(license_key);
  std::string log_path = wrapper.GetLogPath();
  common::logging::INIT_LOGGER(STREAMER_SERVICE_NAME, log_path,
                               common::logging::LOG_LEVEL_INFO);  // initialization
                                                                  // of logging
                                                                  // system
  NOTICE_LOG() << "Running " PROJECT_VERSION_HUMAN << " in " << (run_as_daemon ? "daemon" : "common") << " mode";

  for (char** env = envp; *env != NULL; env++) {
    char* cur_env = *env;
    INFO_LOG() << cur_env;
  }

  int res = wrapper.Exec(argc, argv);
  NOTICE_LOG() << "Quiting " PROJECT_VERSION_HUMAN;

  err = pidfile.Unlock();
  if (err) {
    ERROR_LOG() << "Failed to unlock pidfile: " << PIDFILE_PATH << "; message: " << err->GetDescription();
    return EXIT_FAILURE;
  }

  err = common::file_system::remove_file(PIDFILE_PATH);
  if (err) {
    WARNING_LOG() << "Can't remove file: " << PIDFILE_PATH << ", error: " << err->GetDescription();
  }
  return res;
}
