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

#include <common/utils.h>

#include <string> // for string

#include <gst/gstpad.h> // for GstPad, GstPadProbeInfo, GstPadProbeReturn

#include "stypes.h"

#define PROBE_IN "in"
#define PROBE_OUT "out"

namespace iptv_cloud {
namespace stream {

class IBaseStream;

struct Consistency {
  Consistency();

  gboolean segment;
  gboolean eos;
  gboolean expect_flush;
  gboolean flushing;
  gboolean saw_stream_start;
  gboolean saw_serialized_event;
};

/**
 * @brief A probe structure
 * Defines some supporting variables for probes
 */
class Probe {
public:
  Probe(const std::string &name, element_id_t id, IBaseStream *stream);
  ~Probe();

  const std::string &GetName() const;
  bool IsBroken() const;
  void SetBroken(bool broked);

  void LinkPads(GstPad *pad);
  element_id_t GetID() const;

  GstPad *GetPad() const;
  Consistency GetConsistency() const;

private:
  DISALLOW_COPY_AND_ASSIGN(Probe);
  static GstPadProbeReturn sink_callback_probe_buffer(GstPad *pad,
                                                      GstPadProbeInfo *info,
                                                      gpointer user_data);
  static GstPadProbeReturn source_callback_probe_buffer(GstPad *pad,
                                                        GstPadProbeInfo *info,
                                                        gpointer user_data);
  static void destroy_callback_probe(gpointer user_data);

  void Link(GstPad *pad);
  void Clear();
  void ClearInner();

  IBaseStream *const stream_;

  //! name of a probe
  const std::string name_;
  const element_id_t id_;
  gulong id_buffer_;
  GstPad *pad_;
  Consistency consistency_;
  bool is_broken_;
};

} // namespace stream
} // namespace iptv_cloud
