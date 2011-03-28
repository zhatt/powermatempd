


#include <string.h>
#include <stdlib.h>

#include <iostream>

#include "powermate.h"

using namespace std;


const int MIN_ROTATION = 5;
struct mpdstate {
	Powermate* powermate;
	bool playing;
	int position; // Smoothed by MIN_ROTATION
	bool pressed_and_rotated;
};

void mpd_off( struct mpdstate* mpdstate ) {
	cout << "Turn OFF\n";
	mpdstate->playing = false;
	mpdstate->powermate->setLedBrightnessPercent( 0 );
}

void mpd_on( struct mpdstate* mpdstate ) {
	cout << "Turn ON\n";
	mpdstate->playing = true;
	mpdstate->powermate->setLedBrightnessPercent( 100 );
}

void mpd_toggle_on_off( struct mpdstate* mpdstate ) {
	if ( mpdstate->playing ) {
		mpd_off( mpdstate );
	} else {
		mpd_on( mpdstate );
	}
}

void mpd_prev_track( struct mpdstate* mpdstate ) {
	(void) mpdstate;
	cout << "PREV track\n";
}

void mpd_next_track( struct mpdstate* mpdstate ) {
	(void) mpdstate;
	cout << "NEXT track\n";
}

void mpd_vol_up( struct mpdstate* mpdstate ) {
	(void) mpdstate;
	cout << "Volume UP\n";
}
void mpd_vol_down( struct mpdstate* mpdstate ) {
	(void) mpdstate;
	cout << "Volume DOWN\n";
}

void process_state_change( struct mpdstate* mpdstate,
                           const Powermate::State& pmstate,
                           const Powermate::State& pmlaststate ) {
	/*
	 * press - do nothing
	 * release - toggle play, set LED playing state
	 * press and rotate - change volume
	 * rotate - switch song
	 * release after rotate - set LED to playing state
	 */

	bool rotated = abs( pmstate.position_ - mpdstate->position ) > MIN_ROTATION;

	if ( pmstate.pressed_ && ! pmlaststate.pressed_ ) {
		/*
		 * Newly pressed.
		 */

		mpdstate->pressed_and_rotated = false;

	} else if ( ! pmstate.pressed_ && pmlaststate.pressed_ ) {
		/*
		 * Released
		 */
		if ( ! mpdstate->pressed_and_rotated ) {
			mpd_toggle_on_off( mpdstate );

		} else {
			/*
			 * We changed volume.  Nothing more to do.
			 */
		}

	} else if ( rotated ) {
		if ( pmstate.pressed_ ) {
			/*
			 * Press and rotate.
			 */
			mpdstate->pressed_and_rotated = true;

			if ( pmstate.position_ > mpdstate->position ) {
				mpd_vol_up( mpdstate );
			} else {
				mpd_vol_down( mpdstate );
			}

		} else {
			/*
			 * Rotate
			 */
			if ( pmstate.position_ > mpdstate->position ) {
				mpd_next_track( mpdstate );
			} else {
				mpd_prev_track( mpdstate );
			}
		}

		mpdstate->position = pmstate.position_;
	}

}

int main( ) {

	struct mpdstate mpdstate;
	memset( &mpdstate, 0, sizeof(mpdstate) );

	Powermate powermate;
	mpdstate.powermate = &powermate;

	bool success = powermate.openDevice();

	if ( ! success ) {
		cerr << "Unable open Powermate" << endl;
		return 1;
	}

	mpd_off( &mpdstate );

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

