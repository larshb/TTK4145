#include "elev.h"

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
} Elevator;

void elevator_set_lamps(Elevator e) {
	elev_set_floor_indicator(e.floor);
	elev_set_door_open_lamp(e.state == DOORS_OPEN);
	elev_set_stop_lamp(e.state == STOPPED);
    for (int flr = 0; flr < N_FLOORS; flr++)
        elev_set_button_lamp(BUTTON_COMMAND, flr, e.call[flr]);
}

void elevator_reset_floor() {
	if (elev_get_floor_sensor_signal() == -1){
		elev_set_motor_direction(DIRN_DOWN);
		while(elev_get_floor_sensor_signal() == -1);
		elev_set_motor_direction(DIRN_STOP);
	}
}

void elevator_move(Elevator e) {
    if (e.direction == UP)
        elev_set_motor_direction(DIRN_UP);
    else
        elev_set_motor_direction(DIRN_DOWN);
}

void elevator_stop() {
    elev_set_motor_direction(DIRN_STOP);
}

int elevator_should_stop(Elevator e, int req[N_FLOORS][2]) {
    if ((e.floor == 0 && e.direction == DOWN) || (e.floor == N_FLOORS - 1 && e.direction == UP) || e.call[e.floor] || req[e.floor][e.direction])
        return 1;
    else { //no further opposite direction requests
        if (e.direction == UP) {
            for (int flr = e.floor + 1; flr < N_FLOORS; flr++) {
                if (req[flr][UP] || req[flr][DOWN] || e.call[flr])
                    return 0;
            }
        }
        else {
            for (int flr = 0; flr < e.floor; flr++) {
                if (req[flr][DOWN] || req[flr][UP] || e.call[flr])
                    return 0;
            }
        }
        return 1;
    }
    return 0;
}

int elevator_should_advance(Elevator e, int req[N_FLOORS][2]) {
    if (e.direction == UP && e.floor < N_FLOORS - 1) {
        for (int flr = e.floor + 1; flr < N_FLOORS; flr++){
            if(e.call[flr] || req[flr][UP] || req[flr][DOWN]){
                return 1;
            }
        }
    }
    else if (e.direction == DOWN && e.floor > 0) {
        for (int flr = 0; flr < e.floor; flr++){
            if(e.call[flr] || req[flr][DOWN] || req[flr][UP]){
                return 1;
            }
        }
    }
    return 0;
}

char* elevator_state_to_string(Elevator e) {
    switch(e.state) {
        case IDLE:          return "IDLE";
        case MOVING:        return "MOVING";
        case DOORS_OPEN:    return "DOORS_OPEN";
        case STOPPED:       return "STOPPED";
    }
    return "ERROR";
}