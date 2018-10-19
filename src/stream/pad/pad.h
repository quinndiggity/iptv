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

#include <string>

#include <gst/gstobject.h>

#include <common/macros.h>

typedef struct _GstPad GstPad;

namespace iptv_cloud {
namespace stream {
namespace pad {

// wrapper for GstPad
class Pad {
 public:
  explicit Pad(GstPad* pad);

  GstPad* GetGstPad() const;

  std::string GetType() const;
  std::string GetName() const;

  bool IsLinked() const;
  bool IsValid() const;

  void SetProperty(const char* property, bool val);
  void SetProperty(const char* property, gfloat val);
  void SetProperty(const char* property, gdouble val);
  void SetProperty(const char* property, gint8 val);
  void SetProperty(const char* property, guint8 val);
  void SetProperty(const char* property, gint16 val);
  void SetProperty(const char* property, guint16 val);
  void SetProperty(const char* property, gint val);
  void SetProperty(const char* property, guint val);
  void SetProperty(const char* property, gint64 val);
  void SetProperty(const char* property, guint64 val);
  void SetProperty(const char* property, const char* val);
  void SetProperty(const char* property, const std::string& val);
  void SetProperty(const char* property, void* val);

 private:
  DISALLOW_COPY_AND_ASSIGN(Pad);
  GstPad* pad_;
};

}  // namespace pad
}  // namespace stream
}  // namespace iptv_cloud
