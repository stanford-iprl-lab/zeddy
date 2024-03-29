#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string>

#include <x264.h>

class H264Encoder
{
  struct x264_deleter
  {
    void operator()( x264_t* x ) const { x264_encoder_close( x ); }
  };

  std::unique_ptr<x264_t, x264_deleter> encoder_ {};
  x264_picture_t pic_in_ {}, pic_out_ {};
  int frame_size_ {};

  uint16_t width_;
  uint16_t height_;
  uint8_t fps_;

public:
  H264Encoder( const uint16_t width,
               const uint16_t height,
               const uint8_t fps,
               const std::string& preset,
               const std::string& tune );

  std::string_view encode420( std::span<uint8_t> raster );
};
