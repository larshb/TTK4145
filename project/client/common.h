#pragma once

#include "elev.h"
#include "elevator.h"

int common_get_request(int floor, int direction);
void common_set_request(int floor, int direction, int ownership);
void common_set_lamps();
int common_order_available(Elevator* e);