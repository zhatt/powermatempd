
#ifndef MPD_H
#define MPD_H

#include <mpd/connection.h>

#include <string>

class Mpd {
 public:
	Mpd();
	~Mpd();

	bool connect( const std::string& host );

	void off();
	void on();
	void toggleOnOff();
	void toggleOnOff( bool& isOn );
	void previousTrack();
	void nextTrack();
	void deltaVolume( int delta );
	void volumeUp();
	void volumeUp( unsigned& percentOn );
	void volumeDown();
	void volumeDown( unsigned& percentOn );

	void idleBegin();
	bool idleEnd();  // True if player state changed.

	int getFd();

	bool getIsOn() const { return isOn_; }

 private:
	Mpd& operator=( const Mpd& other );
	Mpd( const Mpd& other );

	void printConnectionError( const std::string& prefix );

	mpd_connection* connection_;
	bool isOn_;
	unsigned volumePercent_;

};

#endif // MPD_H
