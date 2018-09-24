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

#include <gst/gstevent.h>

#include "config.h"
#include "ibase_builder_observer.h"
#include "stream/gst_types.h"
#include "stream_struct.h" // for StreamStatus, StreamStruct (ptr only)

namespace iptv_cloud {
namespace stream {

class IBaseBuilder;
class Probe;

enum ExitStatus { EXIT_SELF, EXIT_INNER };

void streams_init(int argc, char **argv, EncoderType enc = CPU);
void streams_deinit();

class IBaseStream : public common::IMetaClassInfo, public IBaseBuilderObserver {
public:
  class IStreamClient {
  public:
    virtual void OnStatusChanged(IBaseStream *stream, StreamStatus status) = 0;
    virtual void OnPipelineEOS(IBaseStream *stream) = 0;
    virtual void OnTimeoutUpdated(IBaseStream *stream) = 0;
    virtual void OnProbeEvent(IBaseStream *stream, Probe *probe,
                              GstEvent *event) = 0;
    virtual void OnSyncMessageReceived(IBaseStream *stream,
                                       GstMessage *message) = 0;
    virtual void OnASyncMessageReceived(IBaseStream *stream,
                                        GstMessage *message) = 0;
    virtual GstPadProbeInfo *
    OnCheckReveivedOutputData(IBaseStream *stream, Probe *probe,
                              GstPadProbeInfo *info) = 0;
    virtual GstPadProbeInfo *OnCheckReveivedData(IBaseStream *stream,
                                                 Probe *probe,
                                                 GstPadProbeInfo *info) = 0;
    virtual void OnInputChanged(const InputUri &uri) = 0;
    virtual void OnPipelineCreated(IBaseStream *stream) = 0;
    virtual ~IStreamClient();
  };

  enum inited_flags {
    INITED_NOTHING = 0x0000,
    INITED_VIDEO = 0x0001,
    INITED_AUDIO = 0x0002
  };
  enum {
    main_timer_msecs = 1000,
    no_data_panic_sec = 60,
    src_timeout_sec = no_data_panic_sec * 2
  };

  // channel_id_t not empty
  IBaseStream(Config *config, IStreamClient *client, StreamStruct *stats);
  virtual const char *ClassName() const override;
  virtual ~IBaseStream();

  ExitStatus Exec();
  void Restart();

  bool IsLive() const;

  void Quit(ExitStatus status);
  StreamStruct *GetStats() const;

  time_t GetElipsedTime() const; // stream life time sec

  StreamStatus GetStatus() const;
  channel_id_t GetID() const;
  StreamType GetType() const;
  std::vector<Probe *> GetProbesOut() const;

  bool IsActive() const;

  virtual void HandleProbeEvent(Probe *probe, GstEvent *event);

  virtual GstPadProbeInfo *CheckProbeData(Probe *probe, GstPadProbeInfo *buff);
  virtual GstPadProbeInfo *CheckProbeDataOutput(Probe *probe,
                                                GstPadProbeInfo *buff);

  void UpdateStats(const Probe *probe, gsize size);

  Config *GetApi() const;

  void LinkInputPad(GstPad *pad, element_id_t id);
  void LinkOutputPad(GstPad *pad, element_id_t id);

  size_t CountInputEOS() const;
  size_t CountOutEOS() const;

  bool
  DumpIntoFile(const common::file_system::ascii_file_string_path &path) const;

protected:
  elements::Element *GetElementByName(const std::string &name) const;

  bool IsAudioInited() const;
  bool IsVideoInited() const;

  void RegisterAudioCaps(SupportedAudioCodecs saudio, GstCaps *caps,
                         element_id_t id);
  void RegisterVideoCaps(SupportedVideoCodecs svideo, GstCaps *caps,
                         element_id_t id);

  void SetAudioInited(bool val);
  void SetVideoInited(bool val);

  virtual void OnInpudSrcPadCreated(common::uri::Url::scheme scheme,
                                    pad::Pad *src_pad,
                                    element_id_t id) override = 0;
  virtual void OnOutputSinkPadCreated(common::uri::Url::scheme scheme,
                                      pad::Pad *sink_pad,
                                      element_id_t id) override = 0;

  virtual IBaseBuilder *CreateBuilder() = 0;

  virtual void PreLoop() = 0;
  virtual void PostLoop(ExitStatus status) = 0;

  virtual void HandleBufferingMessage(GstMessage *message);

  virtual void SetStatus(StreamStatus status);

  virtual gboolean HandleMainTimerTick();
  virtual GstBusSyncReply HandleSyncBusMessageReceived(GstBus *bus,
                                                       GstMessage *message);
  virtual gboolean HandleAsyncBusMessageReceived(GstBus *bus,
                                                 GstMessage *message);

  virtual void OnInputDataFailed();
  virtual void OnInputDataOK();

  virtual void OnOutputDataFailed();
  virtual void OnOutputDataOK();

  GstStateChangeReturn SetPipelineState(GstState state);

  IStreamClient *const client_;

  // pipeline actions
  void Stop();
  void Pause();
  void Play();

private:
  Config *api_;

  std::vector<Probe *> probe_in_;
  std::vector<Probe *> probe_out_;

  void RuntimeCleanUp();
  bool InitPipeLine();
  void ClearOutProbes();
  void ClearInProbes();
  void ResetDataWait();

  static GstBusSyncReply sync_bus_callback(GstBus *bus, GstMessage *message,
                                           gpointer user_data);
  static gboolean main_timer_callback(gpointer user_data);
  static gboolean async_bus_callback(GstBus *bus, GstMessage *message,
                                     gpointer user_data);

  //! Gstreamer loop pointer. You set it up with you custom run-loop.
  GMainLoop *const loop_;
  GstElement *pipeline_;
  elements_line_t pipeline_elements_;

  StreamStatus status_;

  time_t status_tick;
  time_t no_data_panic_tick;

  StreamStruct *const stats_;

  DISALLOW_COPY_AND_ASSIGN(IBaseStream);

  ExitStatus last_exit_status_;
  bool is_live_;

  int flags_;
  int desire_flags_;
};

} // namespace stream
} // namespace iptv_cloud
