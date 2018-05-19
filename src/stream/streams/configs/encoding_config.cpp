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

#include "stream/streams/configs/encoding_config.h"

#include "stream/constants.h"
#include "stream/gst_types.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

EncodingConfig::EncodingConfig(const base_class& config)
    : base_class(config),
      deinterlace_(false),
      frame_rate_(DEFAULT_FRAME_RATE),
      volume_(DEFAULT_VOLUME),
      video_encoder_(DEFAULT_VIDEO_ENCODER),
      audio_encoder_(DEFAULT_AUDIO_ENCODER),
      audio_channels_(DEFAULT_AUDIO_CHANNEL_COUNT),
      video_encoder_args_(),
      video_encoder_str_args_(),
      width_(DEFAULT_VIDEO_WIDTH),
      height_(DEFAULT_VIDEO_HEIGHT),
      video_bitrate_(DEFAULT_VIDEO_BITRATE),
      audio_bitrate_(DEFAULT_AUDIO_BITRATE),
      logo_path_(),
      logo_pos_point_(),
      logo_alpha_(DEFAULT_LOGO_ALPHA),
      decklink_video_mode_(DEFAULT_DECKLINK_VIDEO_MODE),
      aspect_ratio_{DEFAULT_ASPECT_RATIO_NUM, DEFAULT_ASPECT_RATIO_DEN} {}

void EncodingConfig::SetVolume(volume_t volume) {
  volume_ = volume;
}

volume_t EncodingConfig::GetVolume() const {
  return volume_;
}

int EncodingConfig::GetFramerate() const {
  return frame_rate_;
}

void EncodingConfig::SetFrameRate(int rate) {
  frame_rate_ = rate;
}

bool EncodingConfig::GetDeinterlace() const {
  return deinterlace_;
}

void EncodingConfig::SetDeinterlace(bool deinterlace) {
  deinterlace_ = deinterlace;
}

std::string EncodingConfig::GetVideoEncoder() const {
  return video_encoder_;
}

void EncodingConfig::SetVideoEncoder(const std::string& enc) {
  video_encoder_ = enc;
}

std::string EncodingConfig::GetAudioEncoder() const {
  return audio_encoder_;
}

void EncodingConfig::SetAudioEncoder(const std::string& enc) {
  audio_encoder_ = enc;
}

bool EncodingConfig::IsGpu() const {
  const std::string video_enc = GetVideoEncoder();
  EncoderType enc;
  if (GetTypeEncoder(video_enc, &enc)) {
    return enc == GPU_MFX || enc == GPU_VAAPI;
  }

  return false;
}

bool EncodingConfig::IsMfxGpu() const {
  const std::string video_enc = GetVideoEncoder();
  EncoderType enc;
  if (GetTypeEncoder(video_enc, &enc)) {
    return enc == GPU_MFX;
  }

  return false;
}

audio_channel_count_t EncodingConfig::GetAudioChannels() const {
  return audio_channels_;
}

void EncodingConfig::SetAudioChannels(audio_channel_count_t channels) {
  audio_channels_ = channels;
}

int EncodingConfig::GetWidth() const {
  return width_;
}

void EncodingConfig::SetWidth(int w) {
  width_ = w;
}

int EncodingConfig::GetHeight() const {
  return height_;
}

void EncodingConfig::SetHeight(int h) {
  height_ = h;
}

int EncodingConfig::GetVideoBitrate() const {
  return video_bitrate_;
}

void EncodingConfig::SetVideoBitrate(int bitr) {
  video_bitrate_ = bitr;
}

int EncodingConfig::GetAudioBitrate() const {
  return audio_bitrate_;
}

void EncodingConfig::SetAudioBitrate(int bitr) {
  audio_bitrate_ = bitr;
}

video_encoders_args_t EncodingConfig::GetVideoEncoderArgs() const {
  return video_encoder_args_;
}

void EncodingConfig::SetVideoEncoderArgs(const video_encoders_args_t& args) {
  video_encoder_args_ = args;
}

video_encoders_str_args_t EncodingConfig::GetVideoEncoderStrArgs() const {
  return video_encoder_str_args_;
}

void EncodingConfig::SetVideoEncoderStrArgs(const video_encoders_str_args_t& args) {
  video_encoder_str_args_ = args;
}

void EncodingConfig::SetLogoPath(const std::string& path) {
  logo_path_ = common::uri::Url(path);
}

common::uri::Url EncodingConfig::GetLogoPath() const {
  return logo_path_;
}

alpha_t EncodingConfig::GetLogoAlpha() const {
  return logo_alpha_;
}

void EncodingConfig::SetLogoAlpha(alpha_t al) {
  logo_alpha_ = al;
}

common::draw::Point EncodingConfig::GetLogoPos() const {
  return logo_pos_point_;
}

void EncodingConfig::SetLogoPos(const common::draw::Point& point) {
  logo_pos_point_ = point;
}

common::media::Rational EncodingConfig::GetAspectRatio() const {
  return aspect_ratio_;
}

void EncodingConfig::SetAspectRatio(common::media::Rational rat) {
  aspect_ratio_ = rat;
}

decklink_video_mode_t EncodingConfig::GetDecklinkMode() const {
  return decklink_video_mode_;
}

void EncodingConfig::SetDecklinkMode(decklink_video_mode_t decl) {
  decklink_video_mode_ = decl;
}

PlaylistEncodingConfig::PlaylistEncodingConfig(const base_class& config) : base_class(config), loop_(false) {}

void PlaylistEncodingConfig::SetLoop(bool loop) {
  loop_ = loop;
}

bool PlaylistEncodingConfig::GetLoop() const {
  return loop_;
}

}  // namespace streams
}  // namespace stream
}  // namespace iptv_cloud
