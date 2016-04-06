#include "elevator.h"
#include "common.h"

#include <stdio.h> // printf 
#include <pthread.h> // POSIX threads
#include <semaphore.h> // for what?
#include <string.h>
#include <unistd.h> // sleep

//Debug
#include "debug.h"


static Elevator elevator;
static int common_request[N_FLOORS][2];

void* button_monitor() {
    int floor_result;
    while (!elev_get_stop_signal()) {
        floor_result = elev_get_floor_sensor_signal();
        if (floor_result != -1)
            elevator.floor = floor_result;
        common_set_lamps(common_request);
        elevator_set_lamps(&elevator);
        for (int flr = 0; flr < N_FLOORS; flr++){
            if (elev_get_button_signal(BUTTON_CALL_UP,flr) == 1 && flr != N_FLOORS -1){
                common_request[flr][0] = 1;
            }
            if (elev_get_button_signal(BUTTON_CALL_DOWN,flr) == 1 && flr != 0){
                common_request[flr][1] = 1;
            }
            if (elev_get_button_signal(BUTTON_COMMAND,flr) == 1){
                elevator.call[flr] = 1;
            }
        }
        if (elev_get_stop_signal()){
            elev_set_motor_direction(DIRN_STOP);
            elevator.state = STOPPED;
        }
    }
    return 0;
}

void initialize(){
    printf("Initializing... ");
    elev_set_motor_direction(DIRN_STOP);
    elev_init(ET_Comedi); // ET_Comedi or ET_Simulation
    elevator_initialize(&elevator);
    common_initialize(common_request);
    printf("done\n");
}

void main_test() {
    initialize();
    pthread_t button_monitor_t;
    pthread_create(&button_monitor_t,NULL,button_monitor,"Processing...");
    Elevator_State prev_state = elevator.state;
    int prev_floor = elevator.floor;
    int state_iterator = 1;
    debug_print_state(&state_iterator, &elevator, common_request);
    while (!elev_get_stop_signal()) {
        if (elevator.state != prev_state) {
            debug_print_state(&state_iterator, &elevator, common_request);
            prev_state = elevator.state;
        }
        switch (elevator.state) {
            case IDLE:
            //current floor call or request in same direction
            if (common_request[elevator.floor][elevator.direction] || elevator.call[elevator.floor]) {
                elevator.state = DOORS_OPEN;
                elevator.call[elevator.floor] = 0;
                common_request[elevator.floor][elevator.direction] = 0;
                sleep(1);
            }
            else if (elevator_should_advance(&elevator, common_request)) {
                prev_floor = elevator.floor;
                elevator_move(&elevator);
                elevator.state = MOVING;
            }
            else
                elevator.direction = !elevator.direction;
            break;
            case MOVING:
            if (elevator.floor != prev_floor && elevator_should_stop(&elevator, common_request)) {
                elevator_stop();
                elevator.state = DOORS_OPEN;
                elevator.call[elevator.floor] = 0;
                common_request[elevator.floor][elevator.direction] = 0;
                sleep(1);
            }
            break;
            case DOORS_OPEN:
            if (elevator.call[elevator.floor] || common_request[elevator.floor][elevator.direction]) {
                elevator.call[elevator.floor] = 0;
                common_request[elevator.floor][elevator.direction] = 0;
                sleep(1);
            }
            else if (elevator_should_advance(&elevator, common_request)) {
                prev_floor = elevator.floor;
                elevator_move(&elevator);
                elevator.state = MOVING;
            }
            else
                elevator.state = IDLE;
            break;
            case STOPPED:
            break;
        }
    }
    pthread_join(button_monitor_t,NULL); //Kill monitor
    elev_set_motor_direction(DIRN_STOP);
}

int main(){
    main_test();
}
