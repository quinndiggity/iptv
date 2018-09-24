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

#include <common/draw/types.h>

#include "stream/streams/configs/audio_video_config.h"

#include "stream/stypes.h"

namespace iptv_cloud {
namespace stream {
namespace streams {

class EncodingConfig : public AudioVideoConfig {
public:
  typedef AudioVideoConfig base_class;
  explicit EncodingConfig(const base_class &config);

  volume_t GetVolume() const; // encoding
  void SetVolume(volume_t volume);

  int GetFramerate() const; // encoding
  void SetFrameRate(int rate);

  bool GetDeinterlace() const; // encoding
  void SetDeinterlace(bool deinterlace);

  std::string GetVideoEncoder() const; // encoding
  void SetVideoEncoder(const std::string &enc);

  std::string GetAudioEncoder() const; // encoding
  void SetAudioEncoder(const std::string &enc);

  bool IsGpu() const;    // encoding
  bool IsMfxGpu() const; // encoding

  audio_channel_count_t GetAudioChannels() const; // encoding
  void SetAudioChannels(audio_channel_count_t channels);

  video_encoders_args_t GetVideoEncoderArgs() const; // encoding
  void SetVideoEncoderArgs(const video_encoders_args_t &args);

  video_encoders_str_args_t GetVideoEncoderStrArgs() const; // encoding
  void SetVideoEncoderStrArgs(const video_encoders_str_args_t &args);

  int GetWidth() const; // encoding
  void SetWidth(int w);

  int GetHeight() const; // encoding
  void SetHeight(int h);

  int GetVideoBitrate() const; // encoding
  void SetVideoBitrate(int bitr);

  int GetAudioBitrate() const; // encoding
  void SetAudioBitrate(int bitr);

  common::uri::Url GetLogoPath() const; // encoding
  void SetLogoPath(const std::string &path);

  common::draw::Point GetLogoPos() const; // encoding
  void SetLogoPos(const common::draw::Point &point);

  alpha_t GetLogoAlpha() const;
  void SetLogoAlpha(alpha_t al);

  common::media::Rational GetAspectRatio() const; // encoding
  void SetAspectRatio(common::media::Rational rat);

  decklink_video_mode_t GetDecklinkMode() const; // mosaic
  void SetDecklinkMode(decklink_video_mode_t decl);

private:
  bool deinterlace_;

  int frame_rate_;
  volume_t volume_;

  std::string video_encoder_;
  std::string audio_encoder_;

  int audio_channels_;

  video_encoders_args_t video_encoder_args_;
  video_encoders_str_args_t video_encoder_str_args_;

  int width_;
  int height_;
  int video_bitrate_;
  int audio_bitrate_;

  common::uri::Url logo_path_;
  common::draw::Point logo_pos_point_;
  alpha_t logo_alpha_;

  decklink_video_mode_t decklink_video_mode_;
  common::media::Rational aspect_ratio_;
};

class PlaylistEncodingConfig : public EncodingConfig {
public:
  typedef EncodingConfig base_class;
  explicit PlaylistEncodingConfig(const base_class &config);

  bool GetLoop() const; // enc_playlist relay_playlist
  void SetLoop(bool loop);

private:
  bool loop_;
};

} // namespace streams
} // namespace stream
} // namespace iptv_cloud
