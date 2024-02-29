#pragma once

#include <memory>
#include <span>

extern "C" {
#include "libswscale/swscale.h"
}

class ColorspaceConverter
{
  uint16_t width_, height_;

  struct swscontext_deleter
  {
    void operator()( SwsContext* x ) const { sws_freeContext( x ); }
  };

  std::unique_ptr<SwsContext, swscontext_deleter> context_ {};

public:
  ColorspaceConverter( const uint16_t width, const uint16_t height );

  void convert( std::string_view yuyv422, std::span<uint8_t> yuv420p ) const;
};
