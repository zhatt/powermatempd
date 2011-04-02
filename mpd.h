
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
	void previousTrack();
	void nextTrack();
	void deltaVolume( int delta );
	void volumeUp();
	void volumeDown();

 private:
	Mpd& operator=( const Mpd& other );
	Mpd( const Mpd& other );

	mpd_connection* connection_;
};

#endif // MPD_H
