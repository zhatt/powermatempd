
#include "mpd.h"
#include <iostream>
#include <cassert>
#include <string>

#include <mpd/client.h>

using namespace std;

Mpd::Mpd()
	: connection_( 0 ),
	  isOn_( false ),
	  volumePercent_( 0 )
{}

Mpd::~Mpd() {
	if ( connection_ ) mpd_connection_free ( connection_ );
}

bool Mpd::connect( const string& host ) {
	assert( connection_ == 0 );

	bool success = true;

	connection_ = mpd_connection_new( host.c_str(), 0, 0 );

	if ( connection_ ) {
		mpd_error err = mpd_connection_get_error( connection_ );
		if ( err != MPD_ERROR_SUCCESS ) {
			success = false;
		}
	}

	mpd_status* status = mpd_run_status( connection_ );
	if ( status ) {
		mpd_state state = mpd_status_get_state( status );
		isOn_ = state == MPD_STATE_PLAY;

		cout << "STATE:" << state << endl;

		mpd_status_free( status );
	}
	return success;
}

void Mpd::off() {
	cout << "Turn OFF\n";
	assert( 0 );
}

void Mpd::on() {
	cout << "Turn ON\n";
	assert( 0 ) ;
}

void Mpd::toggleOnOff( bool& isOn ) {
	Mpd::toggleOnOff();
	isOn = isOn_;
}

void Mpd::toggleOnOff() {
	cout << "Toggle ON/OFF\n";

	mpd_status* status = mpd_run_status( connection_ );
	if ( status ) {
		mpd_state state = mpd_status_get_state( status );
		mpd_status_free( status );

		switch( state ) {
		case MPD_STATE_UNKNOWN:
		case MPD_STATE_STOP:
			cout << "Send Play" << endl;
			mpd_run_play( connection_ );
			isOn_ = true;
			break;

		case MPD_STATE_PAUSE:
			cout << "Send pause false" << endl;
			mpd_run_pause( connection_, false );
			isOn_ = true;
			break;

		case MPD_STATE_PLAY:
			cout << "Send pause true" << endl;
			mpd_run_pause( connection_, true );
			isOn_ = false;
			break;
		}
	} else {
		mpd_error err = mpd_connection_get_error( connection_ );
		cerr << "Can't get status: " << err << endl;
	}
}

void Mpd::previousTrack() {
	cout << "PREV track\n";
	mpd_run_previous( connection_ );
}

void Mpd::nextTrack() {
	cout << "NEXT track\n";
	mpd_run_next( connection_ );
}

void Mpd::deltaVolume( int deltaPercent ) {
	mpd_status* status = mpd_run_status( connection_ );
	if ( status ) {
		int volume = mpd_status_get_volume( status );
		mpd_status_free( status );

		cout << "VOL: " << volume << endl;
		volume += deltaPercent;

		if ( volume < 0 ) volume = 0;
		if ( volume > 100 ) volume = 100;

		mpd_run_set_volume( connection_, volume );
		volumePercent_ = volume;

	} else {
		mpd_error err = mpd_connection_get_error( connection_ );
		cerr << "Can't get status: " << err << endl;
	}
}

void Mpd::volumeUp( unsigned& percentOn ) {
	volumeUp();
	percentOn = volumePercent_;
}

void Mpd::volumeUp() {
	deltaVolume( 5 );
	cout << "Volume UP " << volumePercent_ << "\n";
}

void Mpd::volumeDown( unsigned& percentOn ) {
	volumeDown();
	percentOn = volumePercent_;
}

void Mpd::volumeDown() {
	cout << "Volume DOWN " << volumePercent_ << "\n";
	deltaVolume( -5 );
}
