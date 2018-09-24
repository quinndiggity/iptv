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

#include "stream/ibase_builder.h"

#include <gst/gst.h>
#include <gst/gstpipeline.h>

#include <algorithm>

#include "ibase_builder_observer.h"

#include "stream/elements/element.h"
#include "stream/elements/sink/build_output.h"

#include "pad/pad.h"

namespace iptv_cloud {
namespace stream {

IBaseBuilder::IBaseBuilder(Config *api, IBaseBuilderObserver *observer)
    : api_(api), observer_(observer), pipeline_(gst_pipeline_new("pipeline")),
      pipeline_elements_() {}

IBaseBuilder::~IBaseBuilder() {}

elements::Element *
IBaseBuilder::GetElementByName(const std::string &name) const {
  for (elements::Element *el : pipeline_elements_) {
    if (el->GetName() == name) {
      return el;
    }
  }

  NOTREACHED() << "Not founded element name: " << name;
  return nullptr;
}

bool IBaseBuilder::ElementAdd(elements::Element *elem) {
  GstBin *pipeline = GST_BIN(pipeline_);
  bool res = gst_bin_add(pipeline, elem->GetGstElement());
  CHECK(res) << "Can't added " << elem->GetPluginName();
  pipeline_elements_.push_back(elem);
  return res;
}

bool IBaseBuilder::ElementLink(elements::Element *src,
                               elements::Element *dest) {
  bool res = gst_element_link(src->GetGstElement(), dest->GetGstElement());
  CHECK(res) << "Can't linked " << src->GetPluginName() << " to "
             << dest->GetPluginName();
  return res;
}

bool IBaseBuilder::ElementRemove(elements::Element *elem) {
  GstBin *pipeline = GST_BIN(pipeline_);
  bool res = gst_bin_remove(pipeline, elem->GetGstElement());
  CHECK(res);
  pipeline_elements_.erase(
      std::remove(pipeline_elements_.begin(), pipeline_elements_.end(), elem),
      pipeline_elements_.end());
  delete elem;
  return res;
}

bool IBaseBuilder::ElementLinkRemove(elements::Element *src,
                                     elements::Element *dest) {
  gst_element_unlink(src->GetGstElement(), dest->GetGstElement());
  return true;
}

elements::Element *IBaseBuilder::BuildGenericOutput(const OutputUri &output,
                                                    element_id_t sink_id) {
  elements::Element *sink = CreateSink(output, sink_id);
  pad::Pad *sink_pad = sink->StaticPad("sink");
  if (sink_pad->IsValid()) {
    common::uri::Url uri = output.GetOutput();
    common::uri::Url::scheme scheme = uri.GetScheme();
    HandleOutputSinkPadCreated(scheme, sink_pad, sink_id);
  }
  delete sink_pad;
  return sink;
}

elements::Element *IBaseBuilder::CreateSink(const OutputUri &output,
                                            element_id_t sink_id) {
  elements::Element *sink = elements::sink::build_output(output, sink_id);
  return sink;
}

void IBaseBuilder::HandleInputSrcPadCreated(common::uri::Url::scheme scheme,
                                            pad::Pad *pad, element_id_t id) {
  if (observer_) {
    observer_->OnInpudSrcPadCreated(scheme, pad, id);
  }
}

void IBaseBuilder::HandleOutputSinkPadCreated(common::uri::Url::scheme scheme,
                                              pad::Pad *pad, element_id_t id) {
  if (observer_) {
    observer_->OnOutputSinkPadCreated(scheme, pad, id);
  }
}

bool IBaseBuilder::CreatePipeLine(GstElement **pipeline,
                                  elements_line_t *elements) {
  if (!elements) {
    return false;
  }

  if (!InitPipeline()) {
    return false;
  }

  *elements = pipeline_elements_;
  *pipeline = pipeline_;
  return true;
}

} // namespace stream
} // namespace iptv_cloud
