#include <stdio.h>

#include <unistd.h> // sleep

#include "elev.h"

static int elevator_calls[N_FLOORS][2];

typedef enum {
    OFF     = 0,
    ON      = 1
} ES_Lamp_State;

typedef struct {
    int             current_floor;
    ES_Lamp_State   button[N_FLOORS][N_BUTTONS];
    ES_Lamp_State   stop;
    ES_Lamp_State   door_open;
} ES_Panel_Lamps;

typedef enum {
    IDLE,
    DOORS_OPEN,
    MOVING,
    STOPPED
} Elevator_State;

typedef struct {
    int                     floor;
    elev_motor_direction_t  direction;
    Elevator_State          state;
} Elevator;

void setLamps(ES_Panel_Lamps lamps) {
    elev_set_floor_indicator(lamps.current_floor);
    elev_set_door_open_lamp(lamps.door_open);
    elev_set_stop_lamp(lamps.stop);
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < N_BUTTONS; btn++) 
            elev_set_button_lamp(btn, flr, lamps.button[flr][btn]);
    }
}

void gotoFloor(int lvl) {
	if (lvl > elev_get_floor_sensor_signal() && elev_get_floor_sensor_signal() != -1)
		elev_set_motor_direction(DIRN_UP);
	else if (lvl < elev_get_floor_sensor_signal())
		elev_set_motor_direction(DIRN_DOWN);
	while (elev_get_floor_sensor_signal() != lvl){
		if (elev_get_stop_signal())
			break;
	}
	elev_set_motor_direction(DIRN_STOP);
}

void findFloor() {
	if (elev_get_floor_sensor_signal() == -1){
		elev_set_motor_direction(DIRN_DOWN);
		while(elev_get_floor_sensor_signal() == -1);
		elev_set_motor_direction(DIRN_STOP);
	}
}

int _callPressed() {
	for (int i = 0; i < 4; i++){
		if (elev_get_button_signal(2, i))
			return i;
	}
	return -1;
}

void printState() {
	printf("[F]loor [U]p [D]own [C]ommand\n");
	printf("+---+-------+\n| F | U D C |\n+---+-------+\n");
	for (int floor = 3; floor != -1; floor--) {
		printf("| %d |", floor + 1);
		for (int j = 0; j < 3; j++) {
			if (elev_get_button_signal(j, floor))
				printf(" #");
			else
				printf("  ");
		}
		printf(" |\n");
	}
	printf("+---+-------+\n");
}

void _moveTest() {
	int prevFloor = -1;
	int currFloor = -1;
	while(1){
        findFloor();
		currFloor = _callPressed();
		if (currFloor != -1 && currFloor != prevFloor){
			printState();
			gotoFloor(currFloor);
            prevFloor = currFloor;
		}
	}
}

void _disco() {
    ES_Panel_Lamps lamps;
    ES_Lamp_State common_state = OFF;
    lamps.current_floor = 0;
    while(1) {
        lamps.current_floor = (lamps.current_floor + 1) % N_FLOORS;
        common_state = lamps.current_floor % 2;
        for (int flr = 0; flr < N_FLOORS; flr++) {
            for (int btn = 0; btn < N_BUTTONS; btn++)
                lamps.button[flr][btn] = common_state;
        }
        lamps.stop = common_state;
        lamps.door_open = common_state;
        setLamps(lamps);
        sleep(1);
    }
}

int main() {
    elev_init(ET_Comedi); // ET_Comedi or ET_Simulation
    ES_Panel_Lamps lamps;
    lamps.current_floor = 0;
    lamps.stop = OFF;
    lamps.door_open = OFF; 
    for (int flr = 0; flr < N_FLOORS; flr++) {
        elevator_calls[flr][0] = 0;
        elevator_calls[flr][1] = 0;
        for (int btn = 0; btn < N_BUTTONS; btn++)
            lamps.button[flr][btn] = OFF;
    }
    while(1) {
        for (int flr = 0; flr < N_FLOORS; flr++) {
            if (flr != 0 && elev_get_button_signal(BUTTON_CALL_DOWN, flr))
                elevator_calls[flr][1] = 1;
            if (flr != N_FLOORS - 1 && elev_get_button_signal(BUTTON_CALL_UP, flr))
                elevator_calls[flr][0] = 1;
        }
        for (int flr = 0; flr < N_FLOORS; flr++) {
            for (int btn = 0; btn < 2; btn++)
                lamps.button[flr][btn] = elevator_calls[flr][btn];
        }
        setLamps(lamps);
    }
    return 0;
}