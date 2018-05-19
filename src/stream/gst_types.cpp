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

#include "gst_types.h"

#include "gst_constants.h"

namespace iptv_cloud {
namespace stream {

bool GetTypeEncoder(const std::string& encoder, EncoderType* enc) {
  if (encoder.empty() || !enc) {
    return false;
  }

  if (encoder == MFX_H264_ENC) {
    *enc = GPU_MFX;
    return true;
  }

  if (encoder == VAAPI_H264_ENC || encoder == VAAPI_MPEG2_ENC) {
    *enc = GPU_VAAPI;
    return true;
  }

  *enc = CPU;
  return true;
}

}  // namespace stream
}  // namespace iptv_cloud
