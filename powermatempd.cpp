
#include "powermatempd.h"
#include "powermate.h"
#include "mpd.h"

#include <cstdlib>

PowermateMpd::PowermateMpd( Powermate& powermate, Mpd& mpd )
	: powermate_( powermate ),
	  mpd_( mpd ) {}

void PowermateMpd::processStateChange( const Powermate::State& pmState,
                                       const Powermate::State& pmLastState ) {
	/*
	 * press - do nothing
	 * release - toggle play, set LED playing state
	 * press and rotate - change volume
	 * rotate - switch song
	 * release after rotate - set LED to playing state
	 */

	bool rotated = abs( pmState.position_ - position_ ) > minRotation_;

	if ( pmState.pressed_ && ! pmLastState.pressed_ ) {
		/*
		 * Newly pressed.
		 */

		pressedAndRotated_ = false;

	} else if ( ! pmState.pressed_ && pmLastState.pressed_ ) {
		/*
		 * Released
		 */
		if ( ! pressedAndRotated_ ) {
			mpd_.toggleOnOff();

		} else {
			/*
			 * We changed volume.  Nothing more to do.
			 */
		}

	} else if ( rotated ) {
		if ( pmState.pressed_ ) {
			/*
			 * Press and rotate.
			 */
			pressedAndRotated_ = true;

			if ( pmState.position_ > position_ ) {
				mpd_.volumeUp();
			} else {
				mpd_.volumeDown();
			}

		} else {
			/*
			 * Rotate
			 */
			if ( pmState.position_ > position_ ) {
				mpd_.nextTrack();
			} else {
				mpd_.previousTrack();
			}
		}

		position_ = pmState.position_;
	}

}


void PowermateMpd::run() {

	bool result = true;

	Powermate::State state;

	while ( result ) {
		Powermate::State lastState = state;

		result = powermate_.waitForInput( state );
		processStateChange( state, lastState );
	}
}
