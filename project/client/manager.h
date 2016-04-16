#pragma once

#include "elev.h"
#include "assert.h"
#include "elevator.h"
//#include "math.h"

#define MAX_ELEVATORS 16

static Elevator remote_elevator[MAX_ELEVATORS];

int manager_assign(int button_dir, int floor) {
	int chosen_elev_rank = remote_elevator[0].rank;
	Elevator* curr_e;


	//Check idle state
	int last_floor = button_dir == 0 ? 0 : N_FLOORS;
	int flr;

	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		flr = abs(curr_e->floor - floor);
		if (!curr_e->active)
			break;
		if(curr_elev->floor == floor && (curr_elev->state == IDLE || curr_elev->state == DOORS_OPEN)){
			return curr_e->rank;			
		}
		else if (curr_e->state == IDLE && flr < last_floor){
			chosen_elev_rank = curr_e->rank;
			last_floor = flr;
			check = 1;
		}
	}
	if(check == 1){
		return chosen_elev_rank;
	}


	//Check before same direction
	last_floor = button_dir == 0 ? 0 : N_FLOORS;


	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		flr = abs(curr_e->floor - floor);
		if (!curr_e->active)
			break;
		if(button_dir == 0 && curr_e->floor < floor && curr_e->direction == 0 && flr < last_floor){ //Up
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			check = 1;			
		}
		else if(button_dir == 1 && curr_e->floor > floor && curr_e->direction == 1 && flr < last_floor){ //Up
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			check = 1;			
		}
	}
	if(check == 1){
		return chosen_elev_rank;
	}





	assert(chosen_elev_rank != -1);
}
