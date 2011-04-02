
#include "powermatempd.h"
#include "powermate.h"
#include "mpd.h"

#include <cstdlib>

PowermateMpd::PowermateMpd( Powermate& powermate, Mpd& mpd )
	: powermate_( powermate ),
	  mpd_( mpd ) {

	ledOnOff( mpd.getIsOn() );
}

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
			bool isOn;
			mpd_.toggleOnOff( isOn );
			ledOnOff( isOn );

		} else {
			/*
			 * Release after volume change.  Just reset LED state.
			 */

			bool isOn = mpd_.getIsOn();
			ledOnOff( isOn );
		}

	} else if ( rotated ) {
		if ( pmState.pressed_ ) {
			/*
			 * Press and rotate.
			 */
			pressedAndRotated_ = true;

			unsigned volumePercent;

			if ( pmState.position_ > position_ ) {
				mpd_.volumeUp( volumePercent );
			} else {
				mpd_.volumeDown( volumePercent );
			}

			ledPercent( volumePercent );

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

void PowermateMpd::ledOnOff( bool onOff ) {
	ledPercent( onOff ? 100 : 0 );
}

void PowermateMpd::ledPercent( unsigned percent ) {
	powermate_.setLedBrightnessPercent( percent );
}
