#pragma once

#include "elevator.h" // Elevator

#define MAX_ELEVATORS 16

Elevator* manager_get_remote_elevator(int id);
int manager_assign(int floor, int button_dir);