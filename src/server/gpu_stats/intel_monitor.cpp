#include "gpu_stats/intel_monitor.h"

#include <string.h>

#include <cttmetrics.h>

#define METRIC_TIMEOUT_MSEC 1000

namespace iptv_cloud {
namespace gpu_stats {

IntelMonitor::IntelMonitor(int* load) : load_(load), stop_mutex_(), stop_cond_(), stop_flag_(false) {}

IntelMonitor::~IntelMonitor() {}

bool IntelMonitor::IsGpuAvailable() {
  cttStatus status = CTTMetrics_Init();
  if (CTT_ERR_NONE != status) {
    return false;
  }

  CTTMetrics_Close();
  return true;
}

bool IntelMonitor::Exec() {
  if (!load_) {
    return false;
  }

  cttStatus status = CTTMetrics_Init();
  if (CTT_ERR_NONE != status) {
    return false;
  }

  cttMetric metrics_ids[] = {CTT_USAGE_RENDER};
  static const unsigned int metric_cnt = sizeof(metrics_ids) / sizeof(metrics_ids[0]);
  static const unsigned int num_samples = 100;
  static const unsigned int period_ms = METRIC_TIMEOUT_MSEC;
  unsigned int metric_all_cnt = 0;
  status = CTTMetrics_GetMetricCount(&metric_all_cnt);
  if (CTT_ERR_NONE != status) {
    CTTMetrics_Close();
    return false;
  }

  cttMetric metric_all_ids[CTT_MAX_METRIC_COUNT] = {CTT_WRONG_METRIC_ID};
  status = CTTMetrics_GetMetricInfo(metric_all_cnt, metric_all_ids);
  if (CTT_ERR_NONE != status) {
    CTTMetrics_Close();
    return false;
  }

  status = CTTMetrics_Subscribe(metric_cnt, metrics_ids);
  if (CTT_ERR_NONE != status) {
    CTTMetrics_Close();
    return false;
  }

  status = CTTMetrics_SetSampleCount(num_samples);
  if (CTT_ERR_NONE != status) {
    CTTMetrics_Close();
    return false;
  }

  status = CTTMetrics_SetSamplePeriod(period_ms);
  if (CTT_ERR_NONE != status) {
    CTTMetrics_Close();
    return false;
  }

  float metric_values[metric_cnt];
  memset(metric_values, 0, (size_t)metric_cnt * sizeof(float));

  std::unique_lock<std::mutex> lock(stop_mutex_);
  while (!stop_flag_) {
    status = CTTMetrics_GetValue(metric_cnt, metric_values);
    if (CTT_ERR_NONE != status) {
      break;
    }
    *load_ = metric_values[0];

    std::cv_status interrupt_status = stop_cond_.wait_for(lock, std::chrono::seconds(1));
    if (interrupt_status == std::cv_status::no_timeout) {  // if notify
      if (stop_flag_) {
        break;
      }
    }
  }
  CTTMetrics_Close();
  return true;
}

void IntelMonitor::Stop() {
  std::unique_lock<std::mutex> lock(stop_mutex_);
  stop_flag_ = true;
  stop_cond_.notify_one();
}
}  // namespace gpu_stats
}  // namespace iptv_cloud
