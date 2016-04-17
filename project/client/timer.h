#pragma once

#include <sys/time.h>

void timer_set(struct timeval* timeout, int msec);
int timer_timeout(struct timeval* timeout);