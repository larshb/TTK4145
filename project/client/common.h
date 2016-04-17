#pragma once

#include "elevator.h"

void common_init();
void common_complete();
int common_get_request(int floor, int direction);
void common_set_request(int floor, int direction, int ownership);
const char* common_get_next_master_ip();
void common_set_lamps();
int common_order_available(Elevator* e);
void* common_monitor();