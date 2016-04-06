#pragma once

#include "elev.h"
#include "elevator.h"

void common_initialize(int reqs[N_FLOORS][2]);
void common_set_lamps(int reqs[N_FLOORS][2]);
int common_order_available(Elevator* e, int reqs[N_FLOORS][2]);