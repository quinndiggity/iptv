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

#include <gst/gstelement.h>

#include <common/uri/url.h>

#include "stream/config.h"
#include "stream/gst_types.h"
#include "stream/ilinker.h"
#include "stream/stypes.h"

namespace iptv_cloud {
namespace stream {

class IBaseBuilderObserver;

namespace pad {
class Pad;
}

class IBaseBuilder : public ILinker {
public:
  IBaseBuilder(Config *api, IBaseBuilderObserver *observer);
  virtual ~IBaseBuilder();

  bool CreatePipeLine(GstElement **pipeline,
                      elements_line_t *elements) WARN_UNUSED_RESULT;

  elements::Element *GetElementByName(const std::string &name) const;

  virtual bool ElementAdd(elements::Element *elem) override;
  virtual bool ElementLink(elements::Element *src,
                           elements::Element *dest) override;
  virtual bool ElementRemove(elements::Element *elem) override;
  virtual bool ElementLinkRemove(elements::Element *src,
                                 elements::Element *dest) override;

protected:
  elements::Element *BuildGenericOutput(const OutputUri &output,
                                        element_id_t sink_id);
  virtual elements::Element *CreateSink(const OutputUri &output,
                                        element_id_t sink_id);

  virtual bool InitPipeline() WARN_UNUSED_RESULT = 0;

  void HandleInputSrcPadCreated(common::uri::Url::scheme scheme, pad::Pad *pad,
                                element_id_t id);
  void HandleOutputSinkPadCreated(common::uri::Url::scheme scheme,
                                  pad::Pad *pad, element_id_t id);

  Config *const api_;
  IBaseBuilderObserver *const observer_;

private:
  GstElement *const pipeline_;
  elements_line_t pipeline_elements_;
};

} // namespace stream
} // namespace iptv_cloud
