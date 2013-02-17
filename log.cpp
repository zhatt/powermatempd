
#include <cstdio>

#include <syslog.h>

#include "log.h"

static const char* levelToString( LogLevel level ) {

	switch ( level ) {
		case logERROR : return "ERROR";
		case logWARNING : return "WARNING";
		case logINFO : return "INFO";
		case logDEBUG : return "DEBUG";
		case logDEBUG1 : return "DEBUG1";
		case logDEBUG2 : return "DEBUG2";
	}
	/* Won't get here. */ return "";  
}

LogLevel Log::reportingLevel_ = logINFO;

Log::Log() {}

std::ostringstream& Log::get( LogLevel level ) {
//	os << "- " << "NowTime";
	os << " " << levelToString( level ) << ": ";
	os << std::string( level > logDEBUG ? level - logDEBUG : 0, '\t' );
	return os;
}

Log::~Log() {
	os << std::endl;
	syslog( LOG_USER | LOG_INFO, "%s", os.str().c_str() );
	fprintf( stderr, "%s", os.str().c_str() );
	fflush( stderr );
}

