#include "camera.hh"
#include "eventloop.hh"
#include "exception.hh"
#include "h264_encoder.hh"
#include "scale.hh"
#include "stats_printer.hh"

#include <iostream>
#include <span>

using namespace std;

static constexpr unsigned int width = 2560;
static constexpr unsigned int height = 720;
static constexpr unsigned int fps = 30;

void camera_demo( const string& device_name )
{
  auto cam = make_shared<Camera>( width, height, device_name, V4L2_PIX_FMT_YUYV, fps );
  auto loop = make_shared<EventLoop>();
  StatsPrinterTask stats { loop };
  stats.add( cam );
  ColorspaceConverter converter { width, height };
  H264Encoder enc { width, height, fps, "veryfast", "zerolatency" };
  vector<uint8_t> frame420( 3 * width * height / 2, 0 );

  loop->add_rule( "get+convert+encode frame", cam->fd(), Direction::In, [&] {
    auto the_frame_view = cam->borrow_most_recent_frame();
    if ( not the_frame_view.empty() ) {
      converter.convert( the_frame_view, frame420 );
      enc.encode420( frame420 );
    }
    cam->release_frame();
  } );

  while ( loop->wait_next_event( stats.wait_time_ms() ) != EventLoop::Result::Exit ) {}
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    if ( args.size() != 2 ) {
      cerr << "Usage: " << args.front() << " device\n";
      return EXIT_FAILURE;
    }

    const string device_name { args[1] };

    camera_demo( device_name );
  } catch ( const exception& e ) {
    cerr << "Died on exception of type " << demangle( typeid( e ).name() ) << ": " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
