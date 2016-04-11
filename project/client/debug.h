#pragma once

#include "elevator.h"

//void d_goto_floor(int lvl, Elevator* e)
void debug_print_state(int* iteration, Elevator* e, int ownership); /* ownership: 1 = common, 2 = current elevator */