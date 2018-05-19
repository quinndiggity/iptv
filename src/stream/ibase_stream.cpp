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

#include "stream/ibase_stream.h"

#ifdef HAVE_X11
#include <X11/Xlib.h>
#endif

#include <gst/base/gstbasesrc.h>  // for GstBaseSrc

#include <common/time.h>

#include "stream/gstreamer_utils.h"

#include "dumpers/dumpers_factory.h"
#include "stream/ibase_builder.h"
#include "stream/probes.h"  // for Probe (ptr only), PROBE_IN, PROBE_OUT
#include "stream_stats.h"
#include "utils/utils.h"

#include "stream/elements/element.h"

#define MIN_OUT_DATA 4 * 1024 * 60  // 4 kBps
#define MIN_IN_DATA 4 * 1024 * 60   // 4 kBps
#define DEFAULT_FRAMERATE 25

#define MFX_ENV "iHD"
#define MFX_DRIVER_PATH "/opt/intel/mediasdk/lib64"

#define VAAPI_I965_ENV "i965"
#define VAAPI_I965_DRIVER_PATH "/usr/local/lib/dri/"

namespace {

void remove_notify_callback(gpointer user_data) {
  UNUSED(user_data);
}

struct ctx_holder {
  static ctx_holder* instance() {
    static ctx_holder inst;
    return &inst;
  }
  GMainContext* ctx;

 private:
  ctx_holder() {
    ctx = g_main_context_default();  // 1 ref
  }
  ~ctx_holder() {
    g_main_context_unref(ctx);  //
  }
};

void RedirectGstLog(GstDebugCategory* category,
                    GstDebugLevel level,
                    const gchar* file,
                    const gchar* function,
                    gint line,
                    GObject* object,
                    GstDebugMessage* message,
                    gpointer data) {
  UNUSED(data);
  UNUSED(object);

  switch (level) {
    case GST_LEVEL_ERROR:
      ERROR_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                  << gst_debug_message_get(message);
      break;
    case GST_LEVEL_WARNING:
      WARNING_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                    << gst_debug_message_get(message);
      break;
    case GST_LEVEL_FIXME:
      NOTICE_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                   << gst_debug_message_get(message);
      break;
    case GST_LEVEL_INFO:
    case GST_LEVEL_LOG:
      INFO_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                 << gst_debug_message_get(message);
      break;
    case GST_LEVEL_DEBUG:
      DEBUG_LOG() << gst_debug_category_get_name(category) << " " << file << ":" << line << " " << function << " "
                  << gst_debug_message_get(message);
      break;
    default:
      break;
  }
}

}  // namespace

namespace iptv_cloud {
namespace stream {

void streams_init(int argc, char** argv, EncoderType enc) {
  if (gst_is_initialized()) {
    return;
  }

  signal(SIGPIPE, SIG_IGN);
#ifdef HAVE_X11
  XInitThreads();
#endif

  if (enc == GPU_MFX) {
    int res = ::setenv("LIBVA_DRIVER_NAME", MFX_ENV, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVER_NAME to " MFX_ENV;
    }
    res = ::setenv("LIBVA_DRIVERS_PATH", MFX_DRIVER_PATH, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVERS_PATH "
                       "to " MFX_DRIVER_PATH;
    }
  } else if (enc == GPU_VAAPI) {
    int res = ::setenv("LIBVA_DRIVER_NAME", VAAPI_I965_ENV, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVER_NAME "
                       "to " VAAPI_I965_ENV;
    }
    res = ::setenv("LIBVA_DRIVERS_PATH", VAAPI_I965_DRIVER_PATH, 1);
    if (res == ERROR_RESULT_VALUE) {
      WARNING_LOG() << "Failed to set enviroment variable LIBVA_DRIVERS_PATH "
                       "to " VAAPI_I965_DRIVER_PATH;
    }
  }
  if (common::logging::CURRENT_LOG_LEVEL() == common::logging::LOG_LEVEL_DEBUG) {
    int res = ::setenv("GST_DEBUG", "3", 1);
    if (res == SUCCESS_RESULT_VALUE) {
      gst_debug_add_log_function(RedirectGstLog, NULL, NULL);
    }
  }
  gst_init(&argc, &argv);
  const char* va_dr_name = getenv("LIBVA_DRIVER_NAME");
  if (!va_dr_name) {
    va_dr_name = "(null)";
  }
  const char* va_dr_path = getenv("LIBVA_DRIVERS_PATH");
  if (!va_dr_path) {
    va_dr_path = "(null)";
  }
  DEBUG_LOG() << "Stream backend inited, LIBVA_DRIVER_NAME=" << va_dr_name << ", LIBVA_DRIVERS_PATH=" << va_dr_path;
}

void streams_deinit() {
  gst_deinit();
}

IBaseStream::IStreamClient::~IStreamClient() {}

IBaseStream::IBaseStream(Config* config, IStreamClient* client, StreamStruct* stats)
    : common::IMetaClassInfo(),
      client_(client),
      api_(config),
      probe_in_(),
      probe_out_(),
      loop_(g_main_loop_new(ctx_holder::instance()->ctx, FALSE)),
      pipeline_(NULL),
      status_(SNEW),
      status_tick(0),
      no_data_panic_tick(0),
      stats_(stats),
      last_exit_status_(EXIT_INNER),
      is_live_(false),
      flags_(INITED_NOTHING),
      desire_flags_(INITED_NOTHING) {
  /*INFO_LOG() << "Api inited input: " <<
     common::ConvertToString(api_->GetInput())
             << ", output: " << common::ConvertToString(api_->GetOutput());*/
  CHECK(config) << "Config must be!";
  CHECK(stats && stats->IsValid()) << "Stats must be!";
  RuntimeCleanUp();
}

void IBaseStream::LinkInputPad(GstPad* pad, element_id_t id) {
  Probe* probe = new Probe(PROBE_IN, id, this);
  probe->LinkPads(pad);
  probe_in_.push_back(probe);
}

void IBaseStream::LinkOutputPad(GstPad* pad, element_id_t id) {
  Probe* probe = new Probe(PROBE_OUT, id, this);
  probe->LinkPads(pad);
  probe_out_.push_back(probe);
}

void IBaseStream::RuntimeCleanUp() {
  const time_t max_life_time = common::time::current_mstime() / 1000 - 24 * 60 * 60;
  for (const OutputUri& output : api_->GetOutput()) {
    common::uri::Url uri = output.GetOutput();
    common::uri::Url::scheme scheme = uri.GetScheme();

    if (scheme == common::uri::Url::http) {
      const common::file_system::ascii_directory_string_path http_path = output.GetHttpRoot();
      utils::RemoveOldFilesByTime(http_path, max_life_time, CHUNK_EXT);
    }
  }
}

bool IBaseStream::InitPipeLine() {
  IBaseBuilder* builder = CreateBuilder();
  if (!builder->CreatePipeLine(&pipeline_, &pipeline_elements_)) {
    delete builder;
    return false;
  }

  if (client_) {
    client_->OnPipelineCreated(this);
  }

  delete builder;
  return true;
}

void IBaseStream::ClearOutProbes() {
  for (Probe* probe : probe_out_) {
    delete probe;
  }
  probe_out_.clear();
}

void IBaseStream::ClearInProbes() {
  for (Probe* probe : probe_in_) {
    delete probe;
  }
  probe_in_.clear();
}

size_t IBaseStream::CountInputEOS() const {
  size_t count_in_eos = 0;
  std::map<element_id_t, Consistency> probes_statuses;
  for (Probe* probe : probe_in_) {
    element_id_t id = probe->GetID();
    Consistency state = probe->GetConsistency();
    auto it = probes_statuses.find(id);
    if (it != probes_statuses.end()) {
      it->second.eos &= state.eos;
    } else {
      probes_statuses.insert(std::make_pair(id, state));
    }
  }

  for (auto probe_state : probes_statuses) {
    if (probe_state.second.eos) {
      count_in_eos++;
    }
  }

  return count_in_eos;
}

size_t IBaseStream::CountOutEOS() const {
  size_t count_out_eos = 0;
  std::map<element_id_t, Consistency> probes_statuses;
  for (Probe* probe : probe_out_) {
    element_id_t id = probe->GetID();
    Consistency state = probe->GetConsistency();
    auto it = probes_statuses.find(id);
    if (it != probes_statuses.end()) {
      it->second.eos &= state.eos;
    } else {
      probes_statuses.insert(std::make_pair(id, state));
    }
  }

  for (auto probe_state : probes_statuses) {
    if (probe_state.second.eos) {
      count_out_eos++;
    }
  }

  return count_out_eos;
}

const char* IBaseStream::ClassName() const {
  return "IBaseStream";
}

IBaseStream::~IBaseStream() {
  g_main_loop_unref(loop_);
  ClearOutProbes();
  ClearInProbes();
  for (elements::Element* el : pipeline_elements_) {
    delete el;
  }
  pipeline_elements_.clear();
  // pipeline

  SetPipelineState(GST_STATE_NULL);
  if (pipeline_) {
    g_object_unref(pipeline_);
    pipeline_ = NULL;
  }
  destroy(&api_);
}

ExitStatus IBaseStream::Exec() {
  if (!InitPipeLine()) {
    return EXIT_INNER;
  }

  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
  guint main_timeout_id = g_timeout_add(main_timer_msecs, main_timer_callback, this);

  gst_bus_set_sync_handler(bus, sync_bus_callback, this, remove_notify_callback);
  guint bus_watch_id = gst_bus_add_watch(bus, async_bus_callback, this);
  gst_object_unref(bus);
  SetStatus(SNULL);

  stats_->loop_start_time = common::time::current_mstime() / 1000;
  ResetDataWait();

  Play();

  // stream run
  PreLoop();
  g_main_loop_run(loop_);
  PostLoop(last_exit_status_);

  bool res = g_source_remove(bus_watch_id);
  DCHECK(res);
  res = g_source_remove(main_timeout_id);
  DCHECK(res);

  SetStatus(SNULL);  // emulating loop statuses
  Stop();

  stats_->restarts++;
  return last_exit_status_;
}

bool IBaseStream::IsLive() const {
  return is_live_;
}

bool IBaseStream::IsAudioInited() const {
  return flags_ & INITED_AUDIO;
}

bool IBaseStream::IsVideoInited() const {
  return flags_ & INITED_VIDEO;
}

void IBaseStream::RegisterAudioCaps(SupportedAudioCodecs saudio, GstCaps* caps, element_id_t id) {
  UNUSED(saudio);
  bool is_inited_audio = desire_flags_ & INITED_AUDIO;
  if (is_inited_audio) {
    return;
  }

  GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
  if (pad_struct) {
    gint rate = 0, channels = 0;
    if (gst_structure_get_int(pad_struct, "rate", &rate) && gst_structure_get_int(pad_struct, "channels", &channels)) {
      common::media::DesireBytesPerSec kbps = common::media::CalculateDesireAudioBandwidthBytesPerSec(rate, channels);
      input_stream_info_t ins = stats_->input;
      common::media::DesireBytesPerSec prev = ins[id]->GetDesireBytesPerSecond();
      common::media::DesireBytesPerSec next = prev + kbps;
      ins[id]->SetDesireBytesPerSecond(next);
      desire_flags_ |= INITED_AUDIO;
    }
  }
}

void IBaseStream::RegisterVideoCaps(SupportedVideoCodecs svideo, GstCaps* caps, element_id_t id) {
  bool is_inited_video = desire_flags_ & INITED_VIDEO;
  if (is_inited_video) {
    return;
  }

  if (svideo == VIDEO_MPEG_CODEC) {
    GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
    if (pad_struct) {
      gint width = 0;
      gint height = 0;
      if (gst_structure_get_int(pad_struct, "width", &width) && gst_structure_get_int(pad_struct, "height", &height)) {
        common::media::DesireBytesPerSec kbps = common::media::CalculateDesireMPEGBandwidthBytesPerSec(width, height);
        input_stream_info_t ins = stats_->input;
        common::media::DesireBytesPerSec prev = ins[id]->GetDesireBytesPerSecond();
        ins[id]->SetDesireBytesPerSecond(prev + kbps);
        desire_flags_ |= INITED_VIDEO;
      }
    }
  } else if (svideo == VIDEO_H264_CODEC) {
    GstStructure* pad_struct = gst_caps_get_structure(caps, 0);
    if (pad_struct) {
      gint width = 0;
      gint height = 0;
      gint profile = 0;
      if (gst_structure_get_int(pad_struct, "width", &width) && gst_structure_get_int(pad_struct, "height", &height) &&
          gst_structure_get_int(pad_struct, "profile", &profile)) {
        gint framerate = DEFAULT_FRAMERATE;
        gint value_numerator = 0;
        gint value_denominator = 0;

        if (gst_structure_get_fraction(pad_struct, "framerate", &value_numerator, &value_denominator)) {
          framerate = value_numerator / value_denominator;
        }

        common::media::DesireBytesPerSec kbps =
            common::media::CalculateDesireH264BandwidthBytesPerSec(width, height, framerate, profile);
        input_stream_info_t ins = stats_->input;
        common::media::DesireBytesPerSec prev = ins[id]->GetDesireBytesPerSecond();
        ins[id]->SetDesireBytesPerSecond(prev + kbps);
        desire_flags_ |= INITED_VIDEO;
      }
    }
  }
}

void IBaseStream::SetAudioInited(bool val) {
  if (val) {
    flags_ |= INITED_AUDIO;
  } else {
    flags_ &= INITED_AUDIO;
  }
}

void IBaseStream::SetVideoInited(bool val) {
  if (val) {
    flags_ |= INITED_VIDEO;
  } else {
    flags_ &= ~INITED_VIDEO;
  }
}

void IBaseStream::Restart() {
  stats_->loop_start_time = common::time::current_mstime() / 1000;
  ResetDataWait();

  Pause();
  SetStatus(SNULL);  // emulating loop statuses
  Play();

  stats_->restarts++;
}

void IBaseStream::Stop() {
  SetPipelineState(GST_STATE_NULL);
}

void IBaseStream::Pause() {
  SetPipelineState(GST_STATE_PAUSED);
}

void IBaseStream::Play() {
  GstStateChangeReturn ret = SetPipelineState(GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    WARNING_LOG() << "Unable to set the pipeline to the playing state.";
  } else if (ret == GST_STATE_CHANGE_NO_PREROLL) {
    is_live_ = true;
  }
}

void IBaseStream::ResetDataWait() {
  no_data_panic_tick = GetElipsedTime() + no_data_panic_sec;  // update no_data_panic timestamp
  stats_->ResetDataWait();
}

void IBaseStream::Quit(ExitStatus status) {
  GstElement* pipeline = pipeline_;
  GstStructure* result = gst_structure_new("exit_info", "status", G_TYPE_INT, status, NULL);
  GstMessage* message = gst_message_new_application(GST_OBJECT(pipeline), result);
  bool res = gst_element_post_message(pipeline, message);
  if (!res) {
    WARNING_LOG() << GetID() << " Failed to post quit message.";
  }
}

StreamStruct* IBaseStream::GetStats() const {
  return stats_;
}

time_t IBaseStream::GetElipsedTime() const {
  const time_t current_time = common::time::current_mstime() / 1000;
  return current_time - stats_->start_time;
}

StreamStatus IBaseStream::GetStatus() const {
  return status_;
}

channel_id_t IBaseStream::GetID() const {
  return stats_->id;
}

StreamType IBaseStream::GetType() const {
  return stats_->type;
}

std::vector<Probe*> IBaseStream::GetProbesOut() const {
  return probe_out_;
}

GstBusSyncReply IBaseStream::HandleSyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  UNUSED(bus);
  GstMessageType type = GST_MESSAGE_TYPE(message);
  GstObject* src = GST_MESSAGE_SRC(message);
  if (type == GST_MESSAGE_APPLICATION) {
    GstObject* pipeline = GST_OBJECT(pipeline_);
    if (src == pipeline && g_main_loop_is_running(loop_)) {
      const GstStructure* exit_status_struct = gst_message_get_structure(message);
      const GValue* status_val = gst_structure_get_value(exit_status_struct, "status");
      gint exit_status = gvalue_cast<gint>(status_val);
      WARNING_LOG() << GetID() << " Received exit command, status: " << exit_status;
      // DCHECK(g_main_loop_is_running(loop_));
      g_main_loop_quit(loop_);
      last_exit_status_ = static_cast<ExitStatus>(exit_status);
    }
  }

  if (client_) {
    client_->OnSyncMessageReceived(this, message);
  }

  return GST_BUS_PASS;
}

void IBaseStream::OnOutputDataFailed() {
  WARNING_LOG() << GetID() << " There is no output data for a last " << no_data_panic_sec << " seconds.";
  Quit(EXIT_INNER);
}

void IBaseStream::OnOutputDataOK() {
  ResetDataWait();
}

void IBaseStream::OnInputDataFailed() {
  WARNING_LOG() << GetID() << " There is no input data for a last " << no_data_panic_sec << " seconds.";
  Quit(EXIT_INNER);
}

void IBaseStream::OnInputDataOK() {}

gboolean IBaseStream::HandleMainTimerTick() {
  /*for (InputUri input : api_->input()) {
    akamai::AkamaiInfo ainf = input.GetAkamaiInfo();
    if (ainf.isLinkGenerated()) {
      time_t life_time = ainf.linkLifeTime();
      time_t diff_sec = life_time - base::GetUtcNow();
      if (diff_sec < 5) {
        base::WARNING_MSG_FORMAT(CHANNEL_ID_FORMAT " Need to rebuild and play
  new akamai link.", id());
        //pause();

        //play();
      }
    }
  }*/

  const time_t up_time = GetElipsedTime();
  const size_t diff = (no_data_panic_sec - no_data_panic_tick + up_time) + 1;

  size_t checkpoint_diff_in_total = 0;
  common::media::DesireBytesPerSec checkpoint_desire_in_total;
  input_stream_info_t in = stats_->input;
  size_t input_stream_count = in.size();
  for (size_t i = 0; i < input_stream_count; ++i) {
    size_t checkpoint_diff_out_stream = in[i]->GetDiffTotalBytes();
    in[i]->UpdateBps(diff);
    checkpoint_diff_in_total += checkpoint_diff_out_stream;
    checkpoint_desire_in_total += in[i]->GetDesireBytesPerSecond();
  }

  size_t checkpoint_diff_out_total = 0;
  output_stream_info_t out = stats_->output;
  size_t output_stream_count = out.size();
  for (size_t i = 0; i < output_stream_count; ++i) {
    size_t checkpoint_diff_out_stream = out[i]->GetDiffTotalBytes();
    out[i]->UpdateBps(diff);
    checkpoint_diff_out_total += checkpoint_diff_out_stream;
  }

  if (up_time > no_data_panic_tick) {  // check is stream in noraml state
    size_t count_in_eos = CountInputEOS();
    size_t count_out_eos = CountOutEOS();
    DEBUG_LOG() << GetID() << " NoData checkpoint: input eos (" << count_in_eos << "/" << input_stream_count
                << "), output eos (" << count_out_eos << "/" << output_stream_count << "), received bytes "
                << checkpoint_diff_in_total << ", sended bytes " << checkpoint_diff_out_total;
    bool is_input_failed = checkpoint_diff_in_total < MIN_IN_DATA;
    if (is_input_failed) {
      OnInputDataFailed();
    } else {
      OnInputDataOK();
    }

    if (checkpoint_desire_in_total.IsValid() && desire_flags_ != INITED_NOTHING) {
      size_t in_bytes_per_sec = (checkpoint_diff_in_total / diff);
      if (!checkpoint_desire_in_total.InRange(in_bytes_per_sec)) {
        NOTICE_LOG() << "Input bandwidth not in range of desire speed " << checkpoint_desire_in_total.min
                     << " <= " << in_bytes_per_sec << " <= " << checkpoint_desire_in_total.max;
      }
    }

    bool is_output_failed = checkpoint_diff_out_total < MIN_OUT_DATA;
    if (is_output_failed) {
      OnOutputDataFailed();
    } else {
      OnOutputDataOK();
    }
  }

  /*
    Send the update
  */

  if (IsActive()) {
    if (status_tick <= up_time) {
      status_tick = up_time + Config::report_delay_sec;  // update status timestamp
      if (client_) {
        client_->OnTimeoutUpdated(this);
      }
    }
  }

  return TRUE;
}

gboolean IBaseStream::HandleAsyncBusMessageReceived(GstBus* bus, GstMessage* message) {
  UNUSED(bus);

  GstMessageType type = GST_MESSAGE_TYPE(message);
  GstObject* src = GST_MESSAGE_SRC(message);
  switch (type) {
    case GST_MESSAGE_BUFFERING: {
      HandleBufferingMessage(message);
      break;
    }
    case GST_MESSAGE_CLOCK_LOST: {
      GstClock* clock = NULL;
      gst_message_parse_clock_lost(message, &clock);
      INFO_LOG() << GetID() << " ASYNC GST_MESSAGE_CLOCK_LOST: " << GST_OBJECT_NAME(clock);
      break;
    }
    case GST_MESSAGE_DURATION_CHANGED: {
      input_t input = api_->GetInput();
      for (size_t i = 0; i < input.size(); ++i) {
        common::uri::Url input_url = input[i].GetInput();
        if (input_url.GetScheme() == common::uri::Url::http) {
          GstBaseSrc* basesrc = reinterpret_cast<GstBaseSrc*>(src);
          UpdateStats(probe_in_[i], basesrc->segment.duration);
        }
      }
      break;
    }
    case GST_MESSAGE_STATE_CHANGED: {
      GstObject* pipeline = GST_OBJECT(pipeline_);
      if (src == pipeline) {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
        switch (new_state) {
          case GST_STATE_NULL: {
            SetStatus(SNULL);
            break;
          }
          case GST_STATE_READY: {
            SetStatus(SREADY);
            break;
          }
          case GST_STATE_PLAYING: {
            SetStatus(SPLAYING);
            break;
          }
          default:
            break;
        }
      }
      break;
    }
    case GST_MESSAGE_EOS: {
      WARNING_LOG() << GetID() << " Received end of stream: " << GST_OBJECT_NAME(src);
      if (client_) {
        client_->OnPipelineEOS(this);
      }
      break;
    }
    default:
      break;
  }

  if (client_) {
    client_->OnASyncMessageReceived(this, message);
  }

  return TRUE;
}

void IBaseStream::SetStatus(StreamStatus status) {
  if (status_ == status) {
    return;
  }

  INFO_LOG() << GetID() << " Changing status from " << common::ConvertToString(status_) << " to "
             << common::ConvertToString(status);
  status_ = status;
  if (client_) {
    client_->OnStatusChanged(this, status);
  }
}

bool IBaseStream::IsActive() const {
  return status_ != SNULL;
}

void IBaseStream::HandleProbeEvent(Probe* probe, GstEvent* event) {
  if (client_) {
    client_->OnProbeEvent(this, probe, event);
  }
}

elements::Element* IBaseStream::GetElementByName(const std::string& name) const {
  for (elements::Element* el : pipeline_elements_) {
    if (el->GetName() == name) {
      return el;
    }
  }

  NOTREACHED() << "Not founded element name: " << name;
  return nullptr;
}

void IBaseStream::HandleBufferingMessage(GstMessage* message) {
  UNUSED(message);
}

GstPadProbeInfo* IBaseStream::CheckProbeData(Probe* probe, GstPadProbeInfo* buff) {
  if (client_) {
    return client_->OnCheckReveivedData(this, probe, buff);
  }

  return buff;
}

GstPadProbeInfo* IBaseStream::CheckProbeDataOutput(Probe* probe, GstPadProbeInfo* buff) {
  if (client_) {
    return client_->OnCheckReveivedOutputData(this, probe, buff);
  }

  return buff;
}

GstStateChangeReturn IBaseStream::SetPipelineState(GstState state) {
  GstStateChangeReturn ret = gst_element_set_state(pipeline_, state);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    GstState current, pending;
    gst_element_get_state(pipeline_, &current, &pending, 100000);
    WARNING_LOG() << "Unable to change pipeline state from:" << gst_element_state_get_name(current)
                  << " to:" << gst_element_state_get_name(pending);
  }
  return ret;
}

// callbacks

gboolean IBaseStream::main_timer_callback(gpointer user_data) {
  /*
    Restart if no data, set new checkpoint
  */
  IBaseStream* stream = reinterpret_cast<IBaseStream*>(user_data);
  const time_t start_ts = common::time::current_mstime();
  gboolean res = stream->HandleMainTimerTick();
  const time_t end_ts = common::time::current_mstime();
  DEBUG_LOG() << "HandleMainTimerTick time is: " << end_ts - start_ts << " msec.";
  return res;
}

void IBaseStream::UpdateStats(const Probe* probe, gsize size) {
  bool is_broken = probe->IsBroken();
  if (probe->GetName() == PROBE_IN && !is_broken) {
    input_stream_info_t ins = stats_->input;
    if (probe->GetID() < ins.size()) {
      StreamStats* stream_info = ins[probe->GetID()];
      const size_t prev_total = stream_info->GetTotalBytes();
      stream_info->SetTotalBytes(prev_total + size);
      stream_info->SetIsBroken(is_broken);
    }
  } else if (probe->GetName() == PROBE_OUT && !is_broken) {
    output_stream_info_t outs = stats_->output;
    if (probe->GetID() < outs.size()) {
      StreamStats* stream_info = outs[probe->GetID()];
      const size_t prev_total = stream_info->GetTotalBytes();
      stream_info->SetTotalBytes(prev_total + size);
      stream_info->SetIsBroken(is_broken);
    }
  }
}

Config* IBaseStream::GetApi() const {
  return api_;
}

GstBusSyncReply IBaseStream::sync_bus_callback(GstBus* bus, GstMessage* message, gpointer user_data) {
  IBaseStream* stream = reinterpret_cast<IBaseStream*>(user_data);
  return stream->HandleSyncBusMessageReceived(bus, message);
}

gboolean IBaseStream::async_bus_callback(GstBus* bus, GstMessage* message, gpointer user_data) {
  UNUSED(bus);

  IBaseStream* stream = reinterpret_cast<IBaseStream*>(user_data);
  return stream->HandleAsyncBusMessageReceived(bus, message);
}

bool IBaseStream::DumpIntoFile(const common::file_system::ascii_file_string_path& path) const {
  if (!path.IsValid()) {
    return false;
  }

  dumper::IDumper* dumper = dumper::DumpersFactory::GetInstance().CreateDumper(path);
  if (!dumper) {
    return false;
  }

  bool res = dumper->Dump(GST_BIN(pipeline_), path);
  delete dumper;
  return res;
}

}  // namespace stream
}  // namespace iptv_cloud
