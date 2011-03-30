

#include "powermate.h"
#include "mpd.h"
#include "powermatempd.h"

#include <iostream>

using namespace std;

int main( ) {

	Powermate powermate;
	bool success = powermate.openDevice();

	if ( ! success ) {
		cerr << "Unable open Powermate" << endl;
		return 1;
	}

	Mpd mpd;
	success = mpd.connect( "music1." );

	if ( ! success ) {
		cerr << "Unable connect to mpd" << endl;
		return 1;
	}

	PowermateMpd pmmpd( powermate, mpd );

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

