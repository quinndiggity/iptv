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

#include <vector>

#include <common/types.h>

#include <common/file_system/path.h>

namespace iptv_cloud {
namespace utils {

struct CpuShot {
  CpuShot();

  uint64_t user;
  uint64_t nice;
  uint64_t system;
  uint64_t idle;
  uint64_t iowait;
  uint64_t irq;
  uint64_t softirq;
  uint64_t steal;
  uint64_t guest;
  uint64_t guest_nice;
};

long double GetCpuMachineLoad(const CpuShot &prev, const CpuShot &next);
CpuShot GetMachineCpuShot();

struct MemoryShot {
  MemoryShot();

  long double GetAvailable() const;

  uint64_t total_ram; // kb
  uint64_t free_ram;  // kb
  uint64_t avail_ram; // kb
};

MemoryShot GetMachineMemoryShot();

struct HddShot {
  HddShot();

  uint64_t hdd_total; // kb
  uint64_t hdd_free;  // kb
};

HddShot GetMachineHddShot();

struct NetShot {
  NetShot();

  uint64_t bytes_recv; // kb
  uint64_t bytes_send; // kb
};

NetShot GetMachineNetShot();

struct SysinfoShot {
  SysinfoShot();

  unsigned long loads[3];
  long uptime;
};

SysinfoShot GetMachineSysinfoShot();

void RemoveOldFilesByTime(
    const common::file_system::ascii_directory_string_path &dir,
    time_t max_life_secs, const char *ext);

} // namespace utils
} // namespace iptv_cloud
