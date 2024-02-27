#include "camera.hh"
#include "exception.hh"

#include <iostream>
#include <span>

using namespace std;

void camera_demo( [[maybe_unused]] const string& device_name )
{
  throw runtime_error { "this is a test" };
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
