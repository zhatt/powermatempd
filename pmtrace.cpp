
#include "powermate.h"

#include <fcntl.h>
#include <getopt.h>

#include <cassert>
#include <iostream>

using namespace std;

struct Options {
	string device;
	bool traceRaw;
	bool traceEvents;

	Options() : traceRaw( false ), traceEvents( false ) {}
};

static bool parseArgs( Options& options, int argc, char* argv[] ) {
	bool success = true;

	while ( true ) {
		enum {
			DEVICE,
			TRACERAW,
			TRACEEVENTS,
		};

		static const struct option longOptions[] = {
			{ "device", required_argument, 0,  DEVICE },
			{ "traceraw", no_argument,     0,  TRACERAW },
			{ "traceevents", no_argument,  0,  TRACEEVENTS },
			{ 0,	    0,		       0,  0 }
	       };

	       int optionIndex = 0;
	       int c = getopt_long( argc, argv, "", longOptions, &optionIndex);

	       if (c == -1) break;

	       switch (c) {
	       case DEVICE:
		       assert( optarg );
		       options.device = optarg;
		       break;

	       case TRACERAW:
		       options.traceRaw = true;
		       break;

	       case TRACEEVENTS:
		       options.traceEvents = true;
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
	Options options;

	bool success = parseArgs( options, argc, argv );
	if ( ! success ) {
		cerr << "Unable to parse command line" << endl;
		return 1;
	}

	Powermate powermate;
	powermate.setTraceRaw( options.traceRaw );
	powermate.setTraceEvents( options.traceEvents );

	success = powermate.openReadDevice( options.device );

	if ( ! success ) {
		cerr << "Unable open Powermate" << endl;
		return 1;
	}

	Powermate::State state;
	while ( powermate.waitForInput( state ) ) {
	}

	return 0;
}

