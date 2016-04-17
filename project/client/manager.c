#include "manager.h"

#include "pthread.h"	// manager_lock

#include <stdlib.h>		// abs

//debug
//#include "stdio.h"		// printf

static Elevator remote_elevator[MAX_ELEVATORS];

pthread_mutex_t manager_lock;

Elevator* manager_get_remote_elevator(int id) {
	return &remote_elevator[id];
}

int manager_assign(int floor, int button_dir) {
	//printf("---------------------manager_assign(%i, %i);\n", floor, button_dir);
	pthread_mutex_lock(&manager_lock);
	int chosen_elev_rank = remote_elevator[0].rank;
	Elevator* curr_e;


	//Check idle state
	int last_floor =  N_FLOORS;
	int flr;
	int order_assigned = 0;

	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		flr = abs(curr_e->floor - floor);
		if (!curr_e->active){
			break;
		}
		if(curr_e->floor == floor && (curr_e->state == IDLE || curr_e->state == DOORS_OPEN)){
			pthread_mutex_unlock(&manager_lock);
			return curr_e->rank;			
		}
		if (curr_e->state == IDLE && flr < last_floor){
			chosen_elev_rank = curr_e->rank;
			last_floor = flr;
			order_assigned = 1;
		}
	}
	if(order_assigned == 1){
		pthread_mutex_unlock(&manager_lock);
		return chosen_elev_rank;
	}

	//Check before same direction
	last_floor = button_dir == 0 ? 0 : N_FLOORS;


	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		flr = abs(curr_e->floor - floor);
		if (!curr_e->active){
			break;
		}
		if(button_dir == 0 && curr_e->floor < floor && curr_e->direction == 0 && flr < last_floor){ //Up
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			order_assigned = 1;			
		}
		else if(button_dir == 1 && curr_e->floor > floor && curr_e->direction == 1 && flr < last_floor){ //Down
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			order_assigned = 1;			
		}
	}
	if(order_assigned == 1){
		pthread_mutex_unlock(&manager_lock);
		return chosen_elev_rank;
	}


	//Check different direction from end
	int end_floor = button_dir == 0 ? 0 : N_FLOORS;


	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		flr = abs(end_floor - floor);
		if (!curr_e->active){
			break;
		}
		if(button_dir == 0 && curr_e->direction == 1 && flr < last_floor){ //Up
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			order_assigned = 1;			
		}
		else if(button_dir == 1 && curr_e->direction == 0 && flr < last_floor){ //Down
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			order_assigned = 1;			
		}
	}
	if(order_assigned == 1){
		pthread_mutex_unlock(&manager_lock);
		return chosen_elev_rank;
	}


	//Check same direction from end floor
		end_floor = button_dir == 1 ? 0 : N_FLOORS;


	for (int elev_id = 0; elev_id < MAX_ELEVATORS; elev_id++) {
		curr_e = &remote_elevator[elev_id];
		flr = abs(end_floor - floor);
		if (!curr_e->active){
			break;
		}
		if(button_dir == 0 && curr_e->direction == 0 && flr < last_floor){ //Up
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			order_assigned = 1;			
		}
		else if(button_dir == 1 && curr_e->direction == 1 && flr < last_floor){ //Down
			last_floor = flr;
			chosen_elev_rank = curr_e->rank;
			order_assigned = 1;			
		}
	}
	pthread_mutex_unlock(&manager_lock);
	return chosen_elev_rank;
}
