
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

#include <linux/input.h>
#include <assert.h>

static const struct input_event events[] = {
#include "inputtrace.h"
};

int main() {
	unsigned num_events = sizeof(events) / sizeof(events[0]);
	int rfd = open( "/tmp/xxxwrite", O_RDONLY | O_NONBLOCK );  // FIXME
	int wfd = open( "/tmp/xxxread", O_WRONLY );
	assert( rfd != -1 );
	assert( wfd != -1 );

	for ( unsigned i = 0; i < num_events; i++ ) {
		struct timeval tv = events[i].time;

		while ( tv.tv_sec || tv.tv_usec ) {
			fd_set rfds;
			fd_set wfds;

			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(rfd, &rfds);
			//			FD_SET(wfd, &wfds);

			printf("Loop1\n");
			int ret = select((rfd > wfd ? rfd : wfd)+1, &rfds, &wfds, NULL, &tv);
			printf("Select %d\n", ret);
			assert( ret != -1 );

			if ( FD_ISSET( rfd, &rfds ) ) {
				char* buf[2];
				int bytes = read ( rfd, buf, 2 );
				printf("Read %d\n", bytes);
			}

			printf("Loop2\n");
		}
		int ret = write( wfd, &events[i], sizeof(events[i]));
		assert( ret == sizeof(events[i]) );
		printf("write\n");
	}

	return 0;
}
