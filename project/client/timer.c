#include "timer.h"

static struct timeval _current_time; 
static struct timeval _temp_time;

void timer_set(struct timeval* timeout, int msec) {
	_temp_time.tv_sec = msec / 1000;
	_temp_time.tv_usec = (msec % 1000) * 1000;
	gettimeofday(&_current_time, 0);
	timeradd(&_current_time, &_temp_time, timeout);
}

int timer_timeout(struct timeval* timeout) {
	gettimeofday(&_current_time, 0);
	return timercmp(&_current_time, timeout, >);
}