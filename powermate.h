
#ifndef POWERMATE_H
#define POWERMATE_H

#include <string>
#include <vector>

#include <linux/input.h>
#include <fcntl.h>

class Powermate {
 public:
	struct State {
		bool pressed_;
		int position_;

		State() : pressed_( false ), position_( 0 ) {};
	};

	static const unsigned maxInputEventDevices_ = 16;
	static const unsigned maxLedBrightness_ = 255;
	static const unsigned maxPulseSpeed = 510;
	static const unsigned maxPulseTable = 2;

	Powermate();
	~Powermate();

	bool waitForInput( State& state );

	void setLedBrightnessPercent( unsigned percentOn );
	void setAllLedSettings( unsigned staticBrightness, unsigned pulseSpeed,
		                unsigned pulseTable, bool pulseAsleep,
	                        bool pulseAwake );

	bool openDevice();
	bool openDevice( const std::string& device );
	bool openReadDevice( const std::string& device );
	bool openWriteDevice( const std::string& device );

	bool deviceIsOpened() { return readDeviceIsOpened() || writeDeviceIsOpened(); }
	bool readDeviceIsOpened() { return readFd_ != -1; }
	bool writeDeviceIsOpened() { return writeFd_ != -1; }

	void setTraceRaw( bool value ) { traceRaw_ = value; }
	void setTraceEvents( bool value ) { traceEvents_ = value; }

 private:
	Powermate& operator= ( const Powermate& other );
	Powermate( const Powermate& other );

	void processEvent( const input_event& event );
	int readFd_;
	int writeFd_;
	bool pressed_;
	int position_;
	State state_;
	bool traceRaw_;
	bool traceEvents_;

	std::vector<input_event> eventBuffer_;
	std::vector<input_event>::iterator eventBufferNext_;
	std::vector<input_event>::iterator eventBufferLast_;
};

#endif // POWERMATE_H
