#pragma once

#include <condition_variable>

#include "gpu_stats/perf_monitor.h"

namespace iptv_cloud {
namespace gpu_stats {

class IntelMonitor : public IPerfMonitor {
 public:
  explicit IntelMonitor(int* load);
  virtual ~IntelMonitor();

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
