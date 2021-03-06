
#include "mpd.h"
#include "log.h"

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

void Mpd::printConnectionError( const string& prefix ) {
	mpd_error err = mpd_connection_get_error( connection_ );
        const char* errMessage = mpd_connection_get_error_message( connection_ );
	LOG( logERROR ) << prefix
	     << err << ":"
	     << errMessage
	     << endl;
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

		updateIsOn();
	}

	return success;
}

void Mpd::off() {
	LOG( logDEBUG ) << "Turn OFF\n";
	assert( !"Mpd::off() is unimplemented" );
}

void Mpd::on() {
	LOG( logDEBUG ) << "Turn ON\n";
	assert( !"Mpd::on() is unimplemented" ) ;
}

void Mpd::toggleOnOff( bool& isOn ) {
	Mpd::toggleOnOff();
	isOn = isOn_;
}

void Mpd::toggleOnOff() {
	assert( connection_ );

	LOG( logDEBUG ) << "Toggle ON/OFF\n";

	mpd_status* status = mpd_run_status( connection_ );
	if ( status ) {
		mpd_state state = mpd_status_get_state( status );
		mpd_status_free( status );

		switch( state ) {
		case MPD_STATE_UNKNOWN:
		case MPD_STATE_STOP:
			LOG( logDEBUG ) << "Send Play" << endl;
			mpd_run_play( connection_ );
			isOn_ = true;
			break;

		case MPD_STATE_PAUSE:
			LOG( logDEBUG ) << "Send pause false" << endl;
			mpd_run_pause( connection_, false );
			isOn_ = true;
			break;

		case MPD_STATE_PLAY:
			LOG( logDEBUG ) << "Send pause true" << endl;
			mpd_run_pause( connection_, true );
			isOn_ = false;
			break;
		}
	} else {
		printConnectionError("toggleOnOff: ");
	}
}

void Mpd::previousTrack() {
	assert( connection_ );
	LOG( logDEBUG ) << "PREV track\n";
	mpd_run_previous( connection_ );
}

void Mpd::nextTrack() {
	assert( connection_ );
	LOG( logDEBUG ) << "NEXT track\n";
	mpd_run_next( connection_ );
}

void Mpd::deltaVolume( int deltaPercent ) {
	assert( connection_ );
	mpd_status* status = mpd_run_status( connection_ );
	if ( status ) {
		int volume = mpd_status_get_volume( status );
		mpd_status_free( status );

		LOG( logDEBUG ) << "VOL: " << volume << endl;
		volume += deltaPercent;

		if ( volume < 0 ) volume = 0;
		if ( volume > 100 ) volume = 100;

		mpd_run_set_volume( connection_, volume );
		volumePercent_ = volume;

	} else {
		printConnectionError( "deltaVolume: " );
	}
}

void Mpd::volumeUp( unsigned& percentOn ) {
	volumeUp();
	percentOn = volumePercent_;
}

void Mpd::volumeUp() {
	deltaVolume( 5 );
	LOG( logDEBUG ) << "Volume UP " << volumePercent_ << "\n";
}

void Mpd::volumeDown( unsigned& percentOn ) {
	volumeDown();
	percentOn = volumePercent_;
}

void Mpd::volumeDown() {
	LOG( logDEBUG ) << "Volume DOWN " << volumePercent_ << "\n";
	deltaVolume( -5 );
}

void Mpd::idleBegin() {
	assert( connection_ );
	mpd_send_idle_mask( connection_, MPD_IDLE_PLAYER );
}

void Mpd::idleEnd( bool& stateChanged ) {
	assert( connection_ );
	mpd_idle changed = mpd_run_noidle( connection_ );

	bool oldIsOn = isOn_;

	if ( changed ) {
		updateIsOn();
	}

	stateChanged = oldIsOn != isOn_;
}

int Mpd::getFd() {
	assert( connection_ );
	return mpd_connection_get_fd( connection_ );
}

void Mpd::updateIsOn() {
	assert( connection_ );

	mpd_status* status = mpd_run_status( connection_ );
	if ( status ) {
		mpd_state state = mpd_status_get_state( status );
		isOn_ = state == MPD_STATE_PLAY;
		mpd_status_free( status );
	}
}
