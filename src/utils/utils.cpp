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

#include "utils.h"

#include <dirent.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/times.h>

#include <common/file_system/file_system.h>
#include <common/macros.h>
#include <common/sprintf.h>

namespace iptv_cloud {
namespace utils {

void RemoveOldFilesByTime(
    const common::file_system::ascii_directory_string_path &dir,
    time_t max_life_secs, const char *ext) {
  if (!dir.IsValid()) {
    return;
  }

  const std::string path = dir.GetPath();

  DIR *dirp = opendir(path.c_str());
  if (!dirp) {
    return;
  }

  DEBUG_LOG() << "Started clean up hls folder: " << path;
  struct dirent *dent;
  while ((dent = readdir(dirp)) != NULL) {
    if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, "..")) {
      continue;
    }

    char *pch = strstr(dent->d_name, ext);
    if (pch) {
      std::string file_path = common::MemSPrintf("%s%s", path, dent->d_name);
      time_t mtime;
      common::file_system::get_file_time_last_modification(file_path, &mtime);
      if (mtime < max_life_secs) {
        common::ErrnoError err = common::file_system::remove_file(file_path);
        if (err) {
          WARNING_LOG() << "Can't remove file: " << file_path
                        << ", error: " << err->GetDescription();
        } else {
          DEBUG_LOG() << "File path: " << file_path << " removed.";
        }
      }
    }
  }
  closedir(dirp);
  DEBUG_LOG() << "Finished clean up hls folder: " << path;
}

CpuShot::CpuShot()
    : user(0), nice(0), system(0), idle(0), iowait(0), irq(0), softirq(0),
      steal(0), guest(0), guest_nice(0) {}

long double GetCpuMachineLoad(const CpuShot &prev, const CpuShot &next) {
  // http://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  uint64_t PrevIdle = prev.idle + prev.iowait;
  uint64_t Idle = next.idle + next.iowait;

  uint64_t PrevNonIdle = prev.user + prev.nice + prev.system + prev.irq +
                         prev.softirq + prev.steal;
  uint64_t NonIdle = next.user + next.nice + next.system + next.irq +
                     next.softirq + next.steal;

  uint64_t PrevTotal = PrevIdle + PrevNonIdle;
  uint64_t Total = Idle + NonIdle;

  // # differentiate: actual value minus the previous one
  long double totald = Total - PrevTotal;
  long double idled = Idle - PrevIdle;

  return (totald - idled) / totald;
}

CpuShot GetMachineCpuShot() {
  FILE *fp = fopen("/proc/stat", "r");
  if (!fp) {
    return CpuShot();
  }

  CpuShot res;
  unsigned long long int usertime, nicetime, systemtime, idletime;
  char buffer[256];
  unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
  unsigned long long int systemalltime, idlealltime, totaltime, virtalltime;
  UNUSED(systemalltime);
  UNUSED(idlealltime);
  UNUSED(totaltime);
  UNUSED(virtalltime);
  ioWait = irq = softIrq = steal = guest = guestnice = 0;
  // Dependending on your kernel version,
  // 5, 7, 8 or 9 of these fields will be set.
  // The rest will remain at zero.
  fgets(buffer, 255, fp);

  sscanf(buffer,
         "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu "
         "%16llu",
         &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq,
         &steal, &guest, &guestnice);

  // Guest time is already accounted in usertime
  usertime = usertime - guest;
  nicetime = nicetime - guestnice;
  // Fields existing on kernels >= 2.6
  // (and RHEL's patched kernel 2.4...)
  // idlealltime = idletime + ioWait;
  // systemalltime = systemtime + irq + softIrq;
  // virtalltime = guest + guestnice;
  // totaltime = usertime + nicetime + systemalltime + idlealltime + steal +
  // virtalltime;

  res.user = usertime;
  res.nice = nicetime;
  res.system = systemtime;
  res.idle = idletime;
  res.irq = irq;
  res.softirq = softIrq;
  res.steal = steal;
  res.guest = guest;
  res.guest_nice = guestnice;

  fclose(fp);
  return res;
}

MemoryShot::MemoryShot() : total_ram(0), free_ram(0), avail_ram(0) {}

long double MemoryShot::GetAvailable() const {
  return static_cast<long double>(avail_ram) /
         static_cast<long double>(total_ram);
}

MemoryShot GetMachineMemoryShot() {
  FILE *meminfo = fopen("/proc/meminfo", "r");
  if (meminfo == NULL) {
    return MemoryShot();
  }

  char line[256];
  MemoryShot shot;
  while (fgets(line, sizeof(line), meminfo)) {
    if (sscanf(line, "MemTotal: %lu kB", &shot.total_ram) == 1) {
    } else if (sscanf(line, "MemFree: %lu kB", &shot.free_ram) == 1) {
    } else if (sscanf(line, "MemAvailable: %lu kB", &shot.avail_ram) == 1) {
    }
  }

  fclose(meminfo);
  return shot;
}

HddShot::HddShot() : hdd_total(0), hdd_free(0) {}

HddShot GetMachineHddShot() {
  struct statvfs fi_data;
  int res = statvfs("/", &fi_data);
  if (res == ERROR_RESULT_VALUE) {
    return HddShot();
  }

  HddShot sh;
  sh.hdd_total = fi_data.f_blocks * fi_data.f_bsize / 1024;
  sh.hdd_free = fi_data.f_bfree * fi_data.f_bsize / 1024;
  return sh;
}

NetShot::NetShot() : bytes_recv(0), bytes_send(0) {}

NetShot GetMachineNetShot() {
  FILE *netinfo = fopen("/proc/net/dev", "r");
  if (netinfo == NULL) {
    return NetShot();
  }

  NetShot shot;
  char line[512];
  char interf[128] = {0};
  int pos = 0;
  while (fgets(line, sizeof(line), netinfo)) {
    // face |bytes    packets errs drop fifo frame compressed multicast|
    // bytes    packets errs drop fifo colls carrier compressed
    if (pos > 1) {
      unsigned long long int r_bytes, r_packets, r_errs, r_drop, r_fifo,
          r_frame, r_compressed, r_multicast;
      unsigned long long int s_bytes, s_packets, s_errs, s_drop, s_fifo,
          s_colls, s_carrier, s_compressed;
      sscanf(line,
             "%s %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu "
             "%16llu %16llu %16llu "
             "%16llu %16llu %16llu %16llu %16llu",
             interf, &r_bytes, &r_packets, &r_errs, &r_drop, &r_fifo, &r_frame,
             &r_compressed, &r_multicast, &s_bytes, &s_packets, &s_errs,
             &s_drop, &s_fifo, &s_colls, &s_carrier, &s_compressed);
      if (strncmp(interf, "lo", 2) != 0) {
        shot.bytes_recv += r_bytes;
        shot.bytes_send += s_bytes;
      }
      memset(interf, 0, sizeof(interf));
    }
    pos++;
  }

  fclose(netinfo);
  return shot;
}

SysinfoShot::SysinfoShot() : loads{0}, uptime(0) {}

SysinfoShot GetMachineSysinfoShot() {
  struct sysinfo info;
  int res = sysinfo(&info);
  if (res == ERROR_RESULT_VALUE) {
    return SysinfoShot();
  }

  SysinfoShot inf;
  memcpy(&inf.loads, &info.loads,
         sizeof(unsigned long) * SIZEOFMASS(info.loads));
  inf.uptime = info.uptime;
  return inf;
}

} // namespace utils
} // namespace iptv_cloud
