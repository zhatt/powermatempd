
#ifndef __LOG_H__
#define __LOG_H__

#include <sstream>

#define LOG(level) \
	if ( level > Log::reportingLevel() ) ; \
	else Log().get(level)

enum LogLevel {
	logERROR,
	logWARNING,
	logINFO,
	logDEBUG,
	logDEBUG1,
	logDEBUG2
	};

class Log {
public:
	Log();
	virtual ~Log();
	std::ostringstream& get( LogLevel level = logINFO );

	static LogLevel& reportingLevel() { return reportingLevel_; };

protected:
	std::ostringstream os;

private:
	Log( const Log& );
	Log& operator=( const Log& );

	static LogLevel reportingLevel_;

};

#endif  // __LOG_H__

