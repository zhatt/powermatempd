
#include "powermatempd.h"
#include "powermate.h"
#include "mpd.h"
#include "log.h"

#include <iostream>
#include <cstdlib>
#include <poll.h>

using namespace std;

PowermateMpd::PowermateMpd( Powermate& powermate, Mpd& mpd )
	: powermate_( powermate ),
	  mpd_( mpd ),
	  playing_( false ),
	  position_( 0 ),
	  pressed_( false ),
	  pressedAndRotated_( false )
{
	ledOnOff( mpd.getIsOn() );
}

void PowermateMpd::processStateChange( const Powermate::State& newPmState ) {

	/*
	 * press - do nothing
	 * release - toggle play, set LED playing state
	 * press and rotate - change volume
	 * rotate - switch song
	 * release after rotate - set LED to playing state
	 */

	bool rotated = abs( newPmState.position_ - position_ ) > minRotation_;

	if ( newPmState.pressed_ && ! pressed_ ) {
		/*
		 * Newly pressed.
		 */

		pressed_ = true;
		pressedAndRotated_ = false;

	} else if ( ! newPmState.pressed_ && pressed_ ) {
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

		pressed_ = false;
		pressedAndRotated_ = false;

	} else if ( rotated ) {
		if ( newPmState.pressed_ ) {
			/*
			 * Press and rotate.
			 */
			pressedAndRotated_ = true;

			unsigned volumePercent;

			if ( newPmState.position_ > position_ ) {
				mpd_.volumeUp( volumePercent );
			} else {
				mpd_.volumeDown( volumePercent );
			}

			ledPercent( volumePercent );

		} else {
			/*
			 * Rotate
			 */
			if ( newPmState.position_ > position_ ) {
				mpd_.nextTrack();
			} else {
				mpd_.previousTrack();
			}
		}

		position_ = newPmState.position_;
	}
}


void PowermateMpd::run() {

	bool result = true;

	Powermate::State state;

	const unsigned mpdPollFdIndex = 0;
	const unsigned pmPollFdIndex = 1;
	struct pollfd pollFds[2];
	pollFds[mpdPollFdIndex].fd = mpd_.getFd();
	pollFds[mpdPollFdIndex].events = POLLIN;
	pollFds[mpdPollFdIndex].revents = 0;
	pollFds[pmPollFdIndex].fd = powermate_.getReadFd();
	pollFds[pmPollFdIndex].events = POLLIN;
	pollFds[pmPollFdIndex].revents = 0;

	while ( result ) {

		if ( powermate_.hasBufferedEvents() ) {
			result = powermate_.waitForInput( state );
			processStateChange( state );
			continue;
		}

		mpd_.idleBegin();

		poll( pollFds, sizeof(pollFds) / sizeof(pollFds[0]), -1 );

		/*
		 * We need to end the idle on the MPD connection so we can send
		 * it a command below.  Unfortunately due to the way that the
		 * MPD protocol implements idle there is always a chance that
		 * state change events will be lost between idle waits.
		 */
		bool stateChanged;
		mpd_.idleEnd( stateChanged );
		if ( stateChanged ) {
			bool isOn = mpd_.getIsOn();
			LOG( logDEBUG) << "State changed" << " " << isOn << endl;
			ledOnOff( isOn );
		}

		if ( pollFds[pmPollFdIndex].revents & POLLIN ) {
			/*
			 * Powermate has event data to read.
			 */
			result = powermate_.waitForInput( state );
			processStateChange( state );

		} else if ( pollFds[pmPollFdIndex].revents ) {
			/*
			 * Quit if poll reports an error.  POLLERR, POLLHUP, or
			 * POLLNVAL could be set in revents.
			 */
			result = false;
		}
	}
}

void PowermateMpd::ledOnOff( bool onOff ) {
	ledPercent( onOff ? 100 : 0 );
}

void PowermateMpd::ledPercent( unsigned percent ) {
	powermate_.setLedBrightnessPercent( percent );
}
