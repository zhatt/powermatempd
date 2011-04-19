#include "powermate.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <cassert>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

Powermate::Powermate() :
	readFd_( -1 ), writeFd_( -1 ),
	traceRaw_( false ), traceEvents_( false ),
	traceEventStream_( &cout )
{
	eventBuffer_.resize( 32 );
	eventBufferNext_ = eventBufferLast_ = eventBuffer_.end();
}

Powermate::~Powermate() {
	if ( readDeviceIsOpened() ) close( readFd_ );
	if ( writeDeviceIsOpened() && writeFd_ != readFd_ ) close( writeFd_ );
}


static const string griffenNames[] = {
	"Griffin PowerMate",
	"Griffin SoundKnob"
};

bool Powermate::openDevice( const string& device ) {
	assert( ! deviceIsOpened() );
	int trialFd = open( device.c_str(), O_RDWR );
	if ( trialFd != -1 ) readFd_ = writeFd_ = trialFd;
	return trialFd != -1;
}

bool Powermate::openReadDevice( const string& device ) {
	assert( ! deviceIsOpened() );
	int trialFd = open( device.c_str(), O_RDONLY );
	if ( trialFd != -1 ) readFd_ = trialFd;
	return trialFd != -1;
}

bool Powermate::openWriteDevice( const string& device ) {
	assert( ! deviceIsOpened() );
	int trialFd = open( device.c_str(), O_WRONLY );
	if ( trialFd != -1 ) writeFd_ = trialFd;
	return trialFd != -1;
}

bool Powermate::openDevice() {
	assert( ! deviceIsOpened() );
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
						readFd_ = writeFd_ = trialFd;
						break;
					}
				}
			}

			if ( deviceIsOpened() ) break;

			close( trialFd );
		}
	}

	return deviceIsOpened();
}

void Powermate::setLedBrightnessPercent( unsigned percentOn ) {
	unsigned pulseSpeed = 255;
	unsigned pulseTable = 0;
	bool pulseAsleep = 1;
	bool pulseAwake = 0;

	unsigned brightness = percentOn * maxLedBrightness_ / 100;

	setAllLedSettings( brightness, pulseSpeed, pulseTable, pulseAsleep,
	                   pulseAwake);

	if ( traceEvents_ ) {
		getTraceEventStream() <<  "Set LED:" << percentOn << "%\n";
	}
}

void Powermate::setAllLedSettings( unsigned staticBrightness,
	                           unsigned pulseSpeed,
	                           unsigned pulseTable, bool pulseAsleep,
	                           bool pulseAwake) {

	assert( writeDeviceIsOpened() );

	staticBrightness &= 0xFF;

	if ( pulseSpeed > maxPulseSpeed ) pulseSpeed = maxPulseSpeed;
	if ( pulseTable > maxPulseTable ) pulseTable = maxPulseTable;

	struct input_event event;
	memset(&event, 0, sizeof(struct input_event));

	event.type = EV_MSC;
	event.code = MSC_PULSELED;
	event.value = staticBrightness | (pulseSpeed << 8) | (pulseTable << 17) |
		(pulseAsleep << 19) | (pulseAwake << 20);

	if ( traceRaw_ ) {
		ios_base::fmtflags ff = cout.flags();
		cout << hex << setfill('0')
		     << "send_event:  type=0x" << setw(4) << event.type 
		     << ", code=0x" << setw(4) << event.code
		     << ", value=" << dec << event.value << "\n";
		cout.flags( ff );
	}

	int result = write( writeFd_, &event, sizeof(struct input_event) );
	if ( result  != sizeof(struct input_event) ) {
		cerr << "write(): " << strerror(errno) << endl;
	}
}


bool Powermate::waitForInput( Powermate::State& state ) {
	assert( readDeviceIsOpened() );

	bool success = false;

	if ( this->hasBufferedEvents() ) {
		// Return event from previous read.
	} else {
		int bytesRead = read( readFd_, &eventBuffer_[0],
		              sizeof(struct input_event) * eventBuffer_.size() );

		if ( bytesRead > 0 ) {
			unsigned numEvents = bytesRead / sizeof(struct input_event);

			eventBufferNext_ = eventBuffer_.begin();
			eventBufferLast_ = eventBufferNext_ + numEvents;

		} else {
			cerr << "read() failed: " << strerror(errno) << endl;;
			eventBufferNext_ = eventBufferLast_ = eventBuffer_.end();
		}
	}

	if ( eventBufferNext_ != eventBufferLast_ ) {
		processEvent( *eventBufferNext_ );
		++eventBufferNext_;
		success = true;
	}

	state = state_;
	return success;
}

void Powermate::processEvent( const input_event& event ) {

	if ( traceRaw_ ) {
		ios_base::fmtflags ff = cout.flags();
		cout << hex << setfill('0')
		     << "input_event: type=0x" << setw(4) << event.type 
		     << ", code=0x" << setw(4) << event.code
		     << ", value=" << dec << event.value << "\n";
		cout.flags( ff );
	}

	switch( event.type ) {
	case EV_MSC:
		break;

	case EV_REL:
		if ( event.code != REL_DIAL )
			cerr << "Warning: unexpected rotation event" << endl;

		else {
			state_.position_ += event.value;
			if ( traceEvents_ ) {
				getTraceEventStream()
					<< "Button was rotated " << event.value
					<< " units; Offset from start is now "
					<< state_.position_ << " units\n";
			}
		}
		break;

	case EV_KEY:
		if ( event.code != BTN_0 ) {
			cerr << "Warning: unexpected key event" << endl;
		} else {
			state_.pressed_ = event.value;

			if ( traceEvents_ ) {
				cout << "Button was "
				     << ( event.value ? "pressed" : "released" )
				     << endl;
			}
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
