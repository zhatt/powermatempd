
#include "../log.h"

int main() {
	LOG( logINFO ) << "Testing INFO";
	LOG( logERROR ) << "Testing ERROR";
	LOG( logWARNING ) << "Testing WARNING";
	LOG( logINFO ) << "Testing INFO";
	LOG( logDEBUG ) << "Testing DEBUG";
	LOG( logDEBUG1 ) << "Testing DEBUG1";
	LOG( logDEBUG2 ) << "Testing DEBUG2";

	Log::reportingLevel() = logDEBUG2;

	LOG( logINFO ) << "Testing INFO";
	LOG( logERROR ) << "Testing ERROR";
	LOG( logWARNING ) << "Testing WARNING";
	LOG( logINFO ) << "Testing INFO";
	LOG( logDEBUG ) << "Testing DEBUG";
	LOG( logDEBUG1 ) << "Testing DEBUG1";
	LOG( logDEBUG2 ) << "Testing DEBUG2";

	return 0;
}

