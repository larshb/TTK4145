//#include <assert.h>
#include <sys/time.h>

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

/*
#include <time.h>

typedef struct {
	clock_t timeout;
} Timer;

void timer_set(Timer* timer, int msec) {
	timer->timeout = clock() + msec * CLOCKS_PER_SEC / 1000;
	printf("%d\n", timer->timeout);
}

int timer_timeout(Timer* timer) {
	return timer->timeout < clock();
}
*/


/*
clock_t start = clock(), diff;
ProcessIntenseFunction();
diff = clock() - start;

int msec = diff * 1000 / CLOCKS_PER_SEC;
printf("Time taken %d seconds %d milliseconds", msec/1000, msec%1000);

*/