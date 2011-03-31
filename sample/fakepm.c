
#include <unistd.h>
#include <linux/input.h>
#include <assert.h>

#define NO_OP  .type=0x0000, .code=0x0000, .value=0
#define ROTATE(units)  .type=0x0002, .code=0x0007, .value=(units)
#define BUTTON_PRESSED .type=0x0001, .code=0x0100, .value=1
#define BUTTON_RELEASED   .type=0x0001, .code=0x0100, .value=0

static const struct input_event events[] = {
#include "inputtrace.h"
};

int main() {
	unsigned num_events = sizeof(events) / sizeof(events[0]);

	for ( unsigned i = 0; i < num_events; i++ ) {
		int ret = write( 1, &events[i], sizeof(events[i]));
		assert( ret == sizeof(events[i]) );
	}

	return 0;
}
