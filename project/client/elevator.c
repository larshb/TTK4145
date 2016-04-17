#include "elevator.h"

#include "common.h"         // should_stop, should_advance
#include "tcp_client.h"     // Polling rank

void* elevator_monitor(void* elevator) {
    Elevator *e = elevator;
    elevator_initialize(e);
    int floor_result;
    while (e->state != STOPPED && !elev_get_obstruction_signal()) {
        floor_result = elev_get_floor_sensor_signal();
        if (floor_result != -1 && floor_result != e->floor)
            e->floor = floor_result;
        elevator_set_lamps(e);
        for (int flr = 0; flr < N_FLOORS; flr++){
            if (elev_get_button_signal(BUTTON_COMMAND,flr) == 1){
                if (e->call[flr] != 1) {
                    e->call[flr] = 1;
                }
            }
        }
        if (elev_get_stop_signal()){
            elev_set_motor_direction(DIRN_STOP);
            e->state = STOPPED;
        }
    }
    return 0;
}

/*

pthread_mutex_t call_lock;

int elevator_get_call(Elevator* e, int flr) {
    pthread_mutex_lock(&call_lock);
    //
    pthread_mutex_unlock(&call_lock);
}

void elevator_set_call(Elevator* e, int flr, int val) {
    pthread_mutex_lock(&call_lock);
    //
    pthread_mutex_unlock(&call_lock);
}

*/


void elevator_initialize(Elevator* e) {
    elev_init(ET_Comedi); // ET_Comedi or ET_Simulation
    elev_set_motor_direction(DIRN_STOP);
    elevator_reset_floor();
    for (int flr = 0; flr < N_FLOORS; flr++) {        
        e->call[flr] = 0;
    }
    e->state = IDLE;
    e->floor = elev_get_floor_sensor_signal();
    e->direction = UP;
    e->rank = tcp_get_station_rank();
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
    if ((e->floor == 0 && e->direction == DOWN) || (e->floor == N_FLOORS - 1 && e->direction == UP) || e->call[e->floor] || common_get_request(e->floor, e->direction) == e->rank)
        return 1;
    else { //no further opposite direction requests
        if (e->direction == UP) {
            for (int flr = e->floor + 1; flr < N_FLOORS; flr++) {
                if (common_get_request(flr, UP) == e->rank || common_get_request(flr, DOWN) == e->rank || e->call[flr])
                    return 0;
            }
        }
        else {
            for (int flr = 0; flr < e->floor; flr++) {
                if (common_get_request(flr, DOWN) == e->rank || common_get_request(flr, UP) == e->rank || e->call[flr])
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
            if(e->call[flr] || common_get_request(flr, UP) == e->rank || common_get_request(flr, DOWN) == e->rank){
                return 1;
            }
        }
    }
    else if (e->direction == DOWN && e->floor > 0) {
        for (int flr = 0; flr < e->floor; flr++){
            if(e->call[flr] || common_get_request(flr, DOWN) == e->rank || common_get_request(flr, UP) == e->rank){
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