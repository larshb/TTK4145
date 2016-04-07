#pragma once

#include <sys/time.h>

static struct timeval _current_time; 
static struct timeval _temp_time;

void timer_set(struct timeval* timeout, int msec);
int timer_timeout(struct timeval* timeout);