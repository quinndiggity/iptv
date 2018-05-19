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

#include <condition_variable>

#include "gpu_stats/perf_monitor.h"

namespace iptv_cloud {
namespace gpu_stats {

class NvidiaMonitor : public IPerfMonitor {
 public:
  explicit NvidiaMonitor(int* load);
  virtual ~NvidiaMonitor();

  virtual bool Exec() override;
  virtual void Stop() override;

  static bool IsGpuAvailable();

 private:
  int* load_;
  std::mutex stop_mutex_;
  std::condition_variable stop_cond_;
  bool stop_flag_;
};

}  // namespace gpu_stats
}  // namespace iptv_cloud
