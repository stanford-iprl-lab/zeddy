#include <iostream>
#include <span>

#include "exception.hh"
#include "scale.hh"

using namespace std;

ColorspaceConverter::ColorspaceConverter( const uint16_t width, const uint16_t height )
  : width_( width )
  , height_( height )
  , context_( notnull( "sws_getContext YUYV 4:2:2 => YUV 4:2:0 planar",
                       sws_getContext( width,
                                       height,
                                       AV_PIX_FMT_YUYV422,
                                       width,
                                       height,
                                       AV_PIX_FMT_YUV420P,
                                       SWS_FAST_BILINEAR,
                                       nullptr,
                                       nullptr,
                                       nullptr ) ) )
{
  if ( width_ % 2 or height_ % 2 ) {
    throw runtime_error( "Conversion to 4:2:0 needs even width and height" );
  }
}

void ColorspaceConverter::convert( string_view yuv422, span<uint8_t> yuv420p ) const
{
  const array<const uint8_t*, 1> source_planes { reinterpret_cast<const uint8_t*>( yuv422.data() ) };
  const array<const int, 1> source_strides { width_ * 2 };

  const array<uint8_t*, 3> dest_planes { yuv420p.data(),
                                         yuv420p.data() + width_ * height_,
                                         yuv420p.data() + width_ * height_ + ( width_ / 2 ) * ( height_ / 2 ) };
  const array<const int, 3> dest_strides { width_, width_ / 2, width_ / 2 };

  if ( height_
       != sws_scale( context_.get(),
                     source_planes.data(),
                     source_strides.data(),
                     0,
                     height_,
                     dest_planes.data(),
                     dest_strides.data() ) ) {
    throw runtime_error( "unexpected return value from sws_scale" );
  }
}
