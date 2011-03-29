
#include "powermate.h"

#include <fcntl.h>
#include <getopt.h>

#include <cassert>
#include <iostream>

using namespace std;

bool parseArgs( string& device, int argc, char* argv[] ) {
	bool success = true;

	while (1) {
		static const struct option long_options[] = {
			{ "device", required_argument, 0,  0 },
			{ 0,        0,                 0,  0 }
               };

               int option_index = 0;
               int c = getopt_long( argc, argv, "", long_options, &option_index);

               if (c == -1)
                   break;

               switch (c) {
               case 0:
		       switch ( option_index ) {
		       case 0: // --device
			       assert( optarg );
			       device = optarg;
			       break;

		       default:
			       assert ( 0 );
			       break;
		       }

		       break;

               default:
		       success = false;
		       break;
               }
           }

           if (optind < argc) {
		   success = false;
	   }

	   return success;
}

int main( int argc, char* argv[] ) {
	string device;

	bool success = parseArgs( device, argc, argv );
	if ( ! success ) {
		cerr << "Unable to parse command line" << endl;
		return 1;
	}

	Powermate powermate;
	powermate.setTraceRaw( true );

	success = powermate.openDevice( device, O_RDONLY );

	if ( ! success ) {
		cerr << "Unable open Powermate" << endl;
		return 1;
	}

	Powermate::State state;
	while ( powermate.waitForInput( state ) ) {
	}

	return 0;
}

