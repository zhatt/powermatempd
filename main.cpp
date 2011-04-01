

#include "powermate.h"
#include "mpd.h"
#include "powermatempd.h"

#include <getopt.h>
#include <cassert>
#include <iostream>

using namespace std;

struct Options {
	string host;
	string device;
	bool traceRaw;
	bool traceEvents;
	bool daemon;
	Options() : 
		host( "localhost" ),
		traceRaw( false),
		traceEvents( false ),
		daemon( false )
		{}
};

bool parseArgs( Options& options, int argc, char* argv[] ) {
	bool success = true;

	while ( true ) {
		enum {
			HOST,
			DEVICE,
			TRACERAW,
			TRACEEVENTS,
			DAEMON,
		};

		static const struct option longOptions[] = {
			{ "host", required_argument,   0,  HOST },
			{ "device", required_argument, 0,  DEVICE },
			{ "traceraw", no_argument,     0,  TRACERAW },
			{ "traceevents", no_argument,  0,  TRACEEVENTS },
			{ "daemon", no_argument,       0,  DAEMON },
			{ 0,	    0,		       0,  0 }
	       };

	       int optionIndex = 0;
	       int c = getopt_long( argc, argv, "", longOptions, &optionIndex);

	       if (c == -1) break;

	       switch (c) {
	       case HOST:
		       assert( optarg );
		       options.host = optarg;
		       break;

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

	       case DAEMON:
		       options.daemon = true;
		       break;

	       case ':':
	       case '?':
		       success = false;
		       break;

	       default:
		       // Forgot to implement an option.
		       assert( 0 );
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
	if ( options.device.size() ) {
		success = powermate.openDevice( options.device );
	} else {
		success = powermate.openDevice();
	}

	if ( ! success ) {
		cerr << "Unable open Powermate" << endl;
		return 1;
	}

	powermate.setTraceRaw( options.traceRaw );
	powermate.setTraceEvents( options.traceEvents );

	Mpd mpd;
	success = mpd.connect( options.host );

	if ( ! success ) {
		cerr << "Unable connect to mpd" << endl;
		return 1;
	}

	PowermateMpd pmmpd( powermate, mpd );

	if ( options.daemon ) {
		int ret = daemon( 0, 0 );
		if ( ret == -1 ) {
			cerr << "Unable connect to daemonize" << endl;
			return 1;
		}
	}

	pmmpd.run();

	return 0;
}


/*
 * TODO:
 *
 * Click - toggle on and off
 * Press and turn - volume
 * Turn - change song
 *
 * Turn on led when on turn off led when off.
 *
 * There are two click cases.
 *    Down->Up toggle state.
 *    Down->Motion toggle volume.
 *
 * When changing volume, set LED brightness to percent on.
 *
 * Need a threshold of 5 units before deciding that motion is occuring.
 *    How do I reset this when button is not pressed?  Seperate abs position for up and down.
 *
 * Do events need queued and sent asynchronously to mpd?
 *    Initially, we won't do that.
 *
 * Handle position rollover.
 */

