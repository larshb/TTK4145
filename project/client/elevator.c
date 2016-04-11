#include "elevator.h"
#include "common.h"

void elevator_initialize(Elevator* e) {
    elevator_reset_floor();
    for (int flr = 0; flr < N_FLOORS; flr++) {        
        e->call[flr] = 0;
    }
    e->state = IDLE;
    e->floor = elev_get_floor_sensor_signal();
    e->direction = UP;
}

void elevator_set_lamps(Elevator* e) {
	elev_set_floor_indicator(e->floor);
	elev_set_door_open_lamp(e->state == DOORS_OPEN);
	elev_set_stop_lamp(e->state == STOPPED);
    for (int flr = 0; flr < N_FLOORS; flr++)
        elev_set_button_lamp(BUTTON_COMMAND, flr, e->call[flr]);
}

void elevator_reset_floor() {
	if (elev_get_floor_sensor_signal() == -1){
		elev_set_motor_direction(DIRN_DOWN);
		while(elev_get_floor_sensor_signal() == -1);
		elev_set_motor_direction(DIRN_STOP);
	}
}

void elevator_move(Elevator* e) {
    if (e->direction == UP)
        elev_set_motor_direction(DIRN_UP);
    else
        elev_set_motor_direction(DIRN_DOWN);
}

void elevator_stop() {
    elev_set_motor_direction(DIRN_STOP);
}

void elevator_door_open(Elevator* e) {
    timer_set(&e->door_timeout, DOOR_TIME);
}

int elevator_door_closed(Elevator* e) {
    return timer_timeout(&e->door_timeout);
}

int elevator_should_stop(Elevator* e) {
    if ((e->floor == 0 && e->direction == DOWN) || (e->floor == N_FLOORS - 1 && e->direction == UP) || e->call[e->floor] || common_get_request(e->floor, e->direction) == 2)
        return 1;
    else { //no further opposite direction requests
        if (e->direction == UP) {
            for (int flr = e->floor + 1; flr < N_FLOORS; flr++) {
                if (common_get_request(flr, UP) == 2 || common_get_request(flr, DOWN) == 2 || e->call[flr])
                    return 0;
            }
        }
        else {
            for (int flr = 0; flr < e->floor; flr++) {
                if (common_get_request(flr, DOWN) == 2 || common_get_request(flr, UP) == 2 || e->call[flr])
                    return 0;
            }
        }
        return 1;
    }
    return 0;
}

int elevator_should_advance(Elevator* e) {
    if (e->direction == UP && e->floor < N_FLOORS - 1) {
        for (int flr = e->floor + 1; flr < N_FLOORS; flr++){
            if(e->call[flr] || common_get_request(flr, UP) == 2 || common_get_request(flr, DOWN) == 2){
                return 1;
            }
        }
    }
    else if (e->direction == DOWN && e->floor > 0) {
        for (int flr = 0; flr < e->floor; flr++){
            if(e->call[flr] || common_get_request(flr, DOWN) == 2 || common_get_request(flr, UP) == 2){
                return 1;
            }
        }
    }
    return 0;
}

char* elevator_state_to_string(Elevator* e) {
    switch(e->state) {
        case IDLE:          return "IDLE";
        case MOVING:        return "MOVING";
        case DOORS_OPEN:    return "DOORS_OPEN";
        case STOPPED:       return "STOPPED";
    }
    return "ERROR";
}