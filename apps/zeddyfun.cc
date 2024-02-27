#include "camera.hh"
#include "eventloop.hh"
#include "exception.hh"
#include "stats_printer.hh"

#include <iostream>
#include <span>

using namespace std;

void camera_demo( [[maybe_unused]] const string& device_name )
{
  Camera cam { 2560, 720, device_name, V4L2_PIX_FMT_YUYV };

  RasterYUV420 raster { 2560, 720 };

  auto loop = make_shared<EventLoop>();
  StatsPrinterTask stats { loop };

  loop->add_rule( "get frame", cam.fd(), Direction::In, [&] { cam.get_next_frame( raster ); } );

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
