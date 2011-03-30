
#include "mpd.h"
#include <iostream>

using namespace std;

void Mpd::off() {
	cout << "Turn OFF\n";
}

void Mpd::on() {
	cout << "Turn ON\n";
}

void Mpd::toggleOnOff() {
	cout << "Toggle ON/OFF\n";
}

void Mpd::previousTrack() {
	cout << "PREV track\n";
}

void Mpd::nextTrack() {
	cout << "NEXT track\n";
}

void Mpd::volumeUp() {
	cout << "Volume UP\n";
}

void Mpd::volumeDown() {
	cout << "Volume DOWN\n";
}
