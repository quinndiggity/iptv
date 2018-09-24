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

#include "server/gpu_stats/perf_monitor.h"

#ifdef HAVE_NVML
#include "gpu_stats/nvidia_monitor.h"
#endif
#ifdef HAVE_CTT_METRICS
#include "gpu_stats/intel_monitor.h"
#endif

#define MAX_LEN 1024

namespace {

bool IsNvidiaGpuAvailable() {
#ifdef HAVE_NVML
  return iptv_cloud::gpu_stats::NvidiaMonitor::IsGpuAvailable();
#else
  return false;
#endif
}

bool IsIntelGpuAvailable() {
#ifdef HAVE_CTT_METRICS
  return iptv_cloud::gpu_stats::IntelMonitor::IsGpuAvailable();
#else
  return false;
#endif
}
} // namespace

namespace iptv_cloud {
namespace server {
namespace gpu_stats {

IPerfMonitor::~IPerfMonitor() {}

IPerfMonitor *CreatePerfMonitor(int *load) {
  if (IsNvidiaGpuAvailable()) {
#ifdef HAVE_NVML
    return new NvidiaMonitor(load);
#else
    return nullptr;
#endif
  } else if (IsIntelGpuAvailable()) {
#ifdef HAVE_CTT_METRICS
    return new IntelMonitor(load);
#else
    return nullptr;
#endif
  }
  return nullptr;
}

} // namespace gpu_stats
} // namespace server
} // namespace iptv_cloud
