
#ifndef POWERMATEMPD_H
#define POWERMATEMPD_H

#include "powermate.h"
#include "mpd.h"

class PowermateMpd {
 public:
	PowermateMpd( Powermate& powermate, Mpd& mpd );
	void run();

 private:
	void processStateChange( const Powermate::State& pmState,
	                         const Powermate::State& pmLastState );

	static const int minRotation_ = 5;

	Powermate powermate_;
	Mpd mpd_;

	bool playing_;
	bool position_;  // Smoothed by minRotation;
	bool pressedAndRotated_;
};

#endif // POWERMATEMPD_H
