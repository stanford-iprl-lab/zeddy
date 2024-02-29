#include "h264_encoder.hh"
#include "exception.hh"

#include <iostream>
#include <x264.h>

using namespace std;

H264Encoder::H264Encoder( const uint16_t width,
                          const uint16_t height,
                          const uint8_t fps,
                          const string& preset,
                          const string& tune )
  : width_( width ), height_( height ), fps_( fps )
{
  if ( width_ % 2 or height_ % 2 ) {
    throw runtime_error( "H.264 encoder requires even width and height for 4:2:0" );
  }

  // Set params for encoder
  x264_param_t params {};

  if ( x264_param_default_preset( &params, preset.c_str(), tune.c_str() ) != 0 ) {
    throw runtime_error( "Error: Failed to set preset on x264." );
  }

  params.i_threads = 6;
  params.i_width = width_;
  params.i_height = height_;
  params.i_fps_num = fps_;
  params.i_fps_den = 1;
  params.b_annexb = 1;
  params.b_repeat_headers = 1;
  params.i_keyint_max = 2 * fps_;
  //  params.b_intra_refresh = true;

  params.rc.i_qp_constant = 30;
  params.rc.i_rc_method = X264_RC_CQP;

  // Apply profile
  if ( x264_param_apply_profile( &params, "high422" ) != 0 ) {
    throw runtime_error( "Error: Failed to set high422 profile on x264." );
  }
  encoder_.reset( notnull( "x264_encoder_open", x264_encoder_open( &params ) ) );

  x264_picture_init( &pic_in_ );
  x264_picture_init( &pic_out_ );
}

string_view H264Encoder::encode420( span<uint8_t> raster )
{
  if ( 3 * width_ * height_ / 2 != raster.size() ) {
    throw runtime_error( "H264Encoder::encode420(): size mismatch. Expected "
                         + to_string( 3 * width_ * height_ / 2 ) + " but got " + to_string( raster.size() ) );
  }

  pic_in_.img.i_csp = X264_CSP_I420;
  pic_in_.img.i_plane = 3;

  pic_in_.img.i_stride[0] = width_;
  pic_in_.img.i_stride[1] = width_ / 2;
  pic_in_.img.i_stride[2] = width_ / 2;

  pic_in_.img.plane[0] = raster.data();
  pic_in_.img.plane[1] = raster.data() + width_ * height_;
  pic_in_.img.plane[2] = raster.data() + width_ * height_ + ( width_ / 2 ) * ( height_ / 2 );

  int nals_count = 0;
  x264_nal_t* nal;

  const auto frame_size = x264_encoder_encode( encoder_.get(), &nal, &nals_count, &pic_in_, &pic_out_ );

  if ( frame_size < 0 ) {
    throw runtime_error( "x264_encoder_encode returned error" );
  }

  if ( not nal or frame_size <= 0 ) {
    return {};
  }

  return { reinterpret_cast<char*>( nal->p_payload ), static_cast<size_t>( frame_size ) };
}
