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

char* elevator_state_to_string(Elevator e) {
    switch(e.state) {
        case IDLE:          return "IDLE";
        case MOVING:        return "MOVING";
        case DOORS_OPEN:    return "DOORS_OPEN";
        case STOPPED:       return "STOPPED";
    }
    return "ERROR";
}

/*
void elevator_go_down(){
    elevator_reset_floor();
    //int currFlr = elev_get_floor_sensor_signal();
    if (elev_get_floor_sensor_signal() > 0){
        elev_set_motor_direction(DIRN_DOWN);
        elevator.direction = DIRN_DOWN; 
        while (elev_get_floor_sensor_signal() != elevator.floor - 1); //makes the program wait
        elev_set_motor_direction(DIRN_STOP);
        elevator.direction = DIRN_STOP;
    }
}

void elevator_go_up(){
    elevator_reset_floor();
    //int currFlr = elev_get_floor_sensor_signal(); 
    if (elev_get_floor_sensor_signal() < 3){
        elev_set_motor_direction(DIRN_UP);
        elevator.direction = DIRN_UP;
        while (elev_get_floor_sensor_signal() != elevator.floor + 1);
        elev_set_motor_direction(DIRN_STOP);
        elevator.direction = DIRN_STOP;
    }
}

void elevator_open_door(){
    lamps.door_open = ON;
    setLamps(lamps);
    sleep(2); //Door open for 2 seconds;
    lamps.door_open = OFF;
    setLamps(lamps);
}
*/