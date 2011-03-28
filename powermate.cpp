#include "powermate.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <cassert>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <sstream>

using namespace std;

Powermate::Powermate() :
	fd_( -1 ), pressed_( false ), position_( 0 ) {
	eventBuffer.resize( 32 );
	eventBufferNext = eventBufferLast = eventBuffer.end();
}

Powermate::~Powermate() {
	if ( fd_ != -1 ) {
		close( fd_ );
	}
}


static const string griffenNames[] = {
	"Griffin PowerMate",
	"Griffin SoundKnob"
};

bool Powermate::openDevice() {
	assert( fd_ == -1 );
	size_t maxGriffenNameLen = 0;

	for ( unsigned i = 0; i < sizeof(griffenNames) / sizeof(griffenNames[0]); i++ ) {
		maxGriffenNameLen = max( maxGriffenNameLen, griffenNames[i].size() );
	}

	for ( unsigned devNum = 0; devNum < maxInputEventDevices_; devNum++ ) {
		ostringstream devName;

		devName << "/dev/input/event";
		devName << devNum;

		int trialFd = open( devName.str().c_str(), O_RDWR );
		if ( trialFd != -1 ) {
			char buf[maxGriffenNameLen+1];

			int result = ioctl( trialFd, EVIOCGNAME(sizeof(buf)), buf );

			if ( result >= 0 ) {
				buf[maxGriffenNameLen] = '\0';

				for ( unsigned i = 0; i < sizeof(griffenNames) / sizeof(griffenNames[0]); i++ ) {
					if ( griffenNames[i].find( buf ) == 0 ) {
						fd_ = trialFd;
						break;
					}
				}
			}

			if ( fd_ != -1 ) break;

			close( trialFd );
		}
	}

	return fd_ != -1;;
}

void Powermate::setLedBrightnessPercent( int percentOn ) {
	int pulseSpeed = 255;
	int pulseTable = 0;
	int pulseAsleep = 1;
	int pulseAwake = 0;

	int brightness = percentOn * maxLedBrightness_ / 100;

	setAllLedSettings( brightness, pulseSpeed, pulseTable, pulseAsleep,
	                   pulseAwake);

}

void Powermate::setAllLedSettings( unsigned staticBrightness,
	                           unsigned pulseSpeed,
	                           unsigned pulseTable, bool pulseAsleep,
	                           bool pulseAwake) {

	staticBrightness &= 0xFF;

	if ( pulseSpeed > maxPulseSpeed ) pulseSpeed = maxPulseSpeed;
	if ( pulseTable > maxPulseTable ) pulseTable = maxPulseTable;

	struct input_event ev;
	memset(&ev, 0, sizeof(struct input_event));

	ev.type = EV_MSC;
	ev.code = MSC_PULSELED;
	ev.value = staticBrightness | (pulseSpeed << 8) | (pulseTable << 17) |
		(pulseAsleep << 19) | (pulseAwake << 20);

	if ( write(fd_, &ev, sizeof(struct input_event)) != sizeof(struct input_event))
		cerr << "write(): " << strerror(errno) << endl;
}


Powermate::State Powermate::waitForInput() {
	if ( eventBufferNext != eventBufferLast ) {
		// Return event from previous read.
	} else {
		int bytesRead = read( fd_, &eventBuffer[0],
		              sizeof(struct input_event) * eventBuffer.size() );

		if ( bytesRead > 0 ) {
			unsigned numEvents = bytesRead / sizeof(struct input_event);

			eventBufferNext = eventBuffer.begin();
			eventBufferLast = eventBufferNext + numEvents;

		} else {
			cerr << "read() failed: " << strerror(errno) << endl;;
			eventBufferNext = eventBufferLast = eventBuffer.end();
		}
	}

	if ( eventBufferNext != eventBufferLast ) {
		processEvent( *eventBufferNext );
		++eventBufferNext;
	}

	return state_;
}

void Powermate::processEvent( const input_event& event ) {
	#if 0
	//FIXME
	cerr << "type=0x%04x, code=0x%04x, value=%d\n",
		ev->type, ev->code, (int)ev->value );
	#endif

	switch( event.type ) {
	case EV_MSC:
		break;

	case EV_REL:
		if ( event.code != REL_DIAL )
			cerr << "Warning: unexpected rotation event" << endl;

		else {
			state_.position_ += event.value;
			cout << "Button was rotated " << event.value
			     << " units; Offset from start is now " << state_.position_
			     << " units" << endl;
		}
		break;

	case EV_KEY:
		if ( event.code != BTN_0 ) {
			cerr << "Warning: unexpected key event" << endl;
		} else {
			cout << "Button was "
			     << ( event.value ? "pressed" : "released" )
			     << endl;

			state_.pressed_ = event.value;
		}
		break;

	case 0:
		/*
		 * Every other event seems to be a zero event.
		 */
		 break;

	default:
		cerr << "Warning: unexpected event type" << endl;
	}
}

