#include <stdio.h>


#include "elev.h"

void setFloor(int lvl){
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

void initialFloor(){
	if (elev_get_floor_sensor_signal() == -1){
		elev_set_motor_direction(DIRN_DOWN);
		while(elev_get_floor_sensor_signal() == -1);
		elev_set_motor_direction(DIRN_STOP);
	}
}

int buttonPressed(){
	for (int i = 0; i < 4; i++){
		if (elev_get_button_signal(2, i))
			return i;
	}
	return -1;
}

void printState(){
	printf("[F]loor [U]p [D]own [C]ommand\n");
	printf("+-+---+\n|F|UDC|\n+-+---+\n");
	for (int floor = 3; floor != -1; floor--) {
		printf("|%d|", floor + 1);
		for (int j = 0; j < 3; j++) {
			if (elev_get_button_signal(j, floor))
				printf("#");
			else
				printf(" ");
		}
		printf("|\n");
	}
	printf("+-+---+\n");
}

int main() {
	elev_init(ET_Simulation);
	int prevFloor = -1;
	int currFloor = -1;
	while(1){
		initialFloor();
		currFloor = buttonPressed();
		if (currFloor != -1 && currFloor != prevFloor){
			printState();
			setFloor(currFloor);
		}
	}
}