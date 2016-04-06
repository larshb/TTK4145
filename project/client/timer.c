#include <assert.h>
#include <sys/time.h>

static struct timeval current_time;

void timer_set(float* timeout, int milli_seconds) {
	gettimeofday(&current_time, 0);
	*timeout = (current_time.tv_sec) * 1000.0;
    *timeout += (current_time.tv_usec) / 1000.0;
	timeout += milli_seconds;
}

int timer_timeout(long* timeout) {
	gettimeofday(&current_time, 0);
	float current_time_int = current_time.tv_sec * 1000 - current_time.tv_usec / 1000;
	printf("%d, %d\n", *timeout, current_time_int);
	return *timeout > current_time_int;
}