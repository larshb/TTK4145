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
	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		if (!curr_e->active)
			break;
		if (curr_e->state == IDLE)
			chosen_elev_rank = curr_e->rank;
	}
	return chosen_elev_rank;















	/*
	int chosen_elev_rank = -1;
	int chosen_floor_idle = -1;
	int chosen_floor_same_dir = -1;
	Elevator* curr_elev;
	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_elev = &remote_elevator[elev_id];
		if (remote_elevator[elev_id].active) {
			// in same floor and idle or doors_open
			if (curr_elev->floor == floor && (curr_elev->state == IDLE || curr_elev->state == DOORS_OPEN)) {
				chosen_elev_rank = curr_elev->rank;
				common_set_request(floor, button_dir, chosen_elev_rank);
				return chosen_elev_rank;
			}
			// If the elevator moves up and button direction up and assignment floor is above elevator? 
			else if (
				        (curr_elev->direction == UP && button_dir == UP && curr_elev->floor < floor)
					 || (curr_elev->direction == DOWN && button_dir == DOWN && curr_elev->floor > floor)
				) {
				if (chosen_floor_same_dir < 0 || abs(curr_elev->floor-floor) < abs(chosen_floor_same_dir-floor)) {
					if 
					chosen_floor_same_dir = curr_elev->floor;
					chosen_elev_rank = curr_elev->rank;
				}
			}
			else if (chosen_floor_same_dir < 0) {
				if ()
			}
		}
		else {
			break;
		}
	}*/
















	// Check if elevator idle or doors_open in same floor


	// Find nearest elevator above if down pressed
	/*
	int it_dir = button_dir ? 1 : -1;
	int flr;
	for (flr = floor + it_dir; floor >= 0 && floor < N_FLOORS; i += it_dir) {
		for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
			if (!remote_elevator[elev_id].active)
				break;
			//
		}
	}*/
	assert(chosen_elev_rank != -1);
}
