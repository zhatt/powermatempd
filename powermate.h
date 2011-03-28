
#ifndef POWERMATE_H
#define POWERMATE_H

#include <string>
#include <vector>

#include <linux/input.h>


class Powermate {
 public:
	struct State {
		bool pressed_;
		int position_;
		bool ledIsOn_;
	};

	static const unsigned maxInputEventDevices_ = 16;
	static const unsigned maxLedBrightness_ = 255;
	static const unsigned maxPulseSpeed = 510;
	static const unsigned maxPulseTable = 2;

	Powermate();
	~Powermate();

	State waitForInput();

	void setLedBrightnessPercent( int percentOn );
	void setAllLedSettings( unsigned staticBrightness, unsigned pulseSpeed,
		                unsigned pulseTable, bool pulseAsleep,
	                        bool pulseAwake );

	bool openDevice();
	bool openDevice( const std::string& device );

 private:
	void processEvent( const input_event& event );
	int fd_;
	bool pressed_;
	int position_;
	State state_;

	std::vector<input_event> eventBuffer;
	std::vector<input_event>::iterator eventBufferNext;
	std::vector<input_event>::iterator eventBufferLast;
};

#endif // POWERMATE_H
