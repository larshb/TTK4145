#pragma once

#include "elev.h"
#include "assert.h"

#define MAX_ELEVATORS 16

static Elevator remote_elevator[MAX_ELEVATORS];
/*
manager_assign(button_dir, floor) {

	int chosen = -1;

	// Find nearest elevator above if down pressed
	int it_dir = button_dir ? 1 : -1;
	int flr;
	for (flr = floor + it_dir; floor >= 0 && floor < N_FLOORS; i += it_dir) {
		for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
			if (!remote_elevator[elev_id].active)
				break;
			//
		}
	}
	assert(chosen != -1);
}
*/