
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

	bool getIsOn() const { return isOn_; }

 private:
	Mpd& operator=( const Mpd& other );
	Mpd( const Mpd& other );

	mpd_connection* connection_;
	bool isOn_;
	unsigned volumePercent_;
};

#endif // MPD_H
