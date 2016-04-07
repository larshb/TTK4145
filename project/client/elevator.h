#pragma once

#include "elev.h"
#include "timer.h"

#define DOOR_TIME 1000 //ms

typedef enum {
    IDLE,
    MOVING,
    DOORS_OPEN,
    STOPPED
} Elevator_State;

typedef enum {
	UP = 0,
	DOWN = 1
} Elevator_Direction;

typedef struct {
    int                     floor;
    int 					call[N_FLOORS];
    Elevator_Direction	  	direction;
    Elevator_State          state;
    struct timeval          door_timeout;
} Elevator;

void elevator_initialize(Elevator* e);
void elevator_set_lamps(Elevator* e);
void elevator_reset_floor();
void elevator_move(Elevator* e);
void elevator_stop();
void elevator_door_open(Elevator* e);
int elevator_door_closed(Elevator* e);
int elevator_should_stop(Elevator* e, int req[N_FLOORS][2]);
int elevator_should_advance(Elevator* e, int req[N_FLOORS][2]);
char* elevator_state_to_string(Elevator* e);