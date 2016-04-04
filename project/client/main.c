#include "elev.h"
#include "elevator.c"

#include <stdio.h>
#include <pthread.h> // POSIX threads
#include <semaphore.h> // for what?
#include <string.h>

//Debug
//#include "debug.c"
#include <unistd.h> // sleep

static Elevator elevator;
static int elevator_request[N_FLOORS][2];

void panel_set_lamps() {
    for (int flr = 0; flr < N_FLOORS - 1; flr++) {
        elev_set_button_lamp(BUTTON_CALL_UP, flr, elevator_request[flr][0]);
        elev_set_button_lamp(BUTTON_CALL_DOWN, flr + 1, elevator_request[flr + 1][1]);
    }
}

int order_available(Elevator e) {
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < 2; btn++) {
            if (elevator_request[flr][btn])
                return 1;
        }
        if (e.call[flr])
            return 1;
    }
    return 0;
}

void d_print_state(int* iteration) {
    printf("\n[F]loor [U]p [D]own [C]ommand\n");
    printf("+---+-------+\n| F | U D C |\n+---+-------+\n");
    for (int flr = 3; flr != -1; flr--) {
        printf("| %d |", flr);
        for (int btn = 0; btn < 2; btn++) {
            if (elevator_request[flr][btn] == 1)
                printf(" #");
            else
                printf("  ");
        }
        if (elevator.call[flr])
            printf(" #");
        else
            printf("  ");
        printf(" |");
        if (flr == elevator.floor) {
            if (elevator.state != MOVING) {
                if (elevator.state == DOORS_OPEN)
                    printf("[ ]");
                else
                    printf("+");
            }
            else {
                if (elevator.direction == UP)
                    printf("^");
                else
                    printf("v");
            }
        }
        printf("\n");
    }
    printf("+---+-------%d\n", (*iteration)++);
    printf("Elevator floor: %d\n", elevator.floor);
    printf("Elevator state: %s\n", elevator_state_to_string(elevator));
}

void *button_monitor() {
    int floor_result;
    while (!elev_get_stop_signal()) {
        floor_result = elev_get_floor_sensor_signal();
        if (floor_result != -1)
            elevator.floor = floor_result;
        panel_set_lamps();
        elevator_set_lamps(elevator);
        for (int flr = 0; flr < N_FLOORS; flr++){
            if (elev_get_button_signal(BUTTON_CALL_UP,flr) == 1 && flr != N_FLOORS -1){
                elevator_request[flr][0] = 1;
            }
            if (elev_get_button_signal(BUTTON_CALL_DOWN,flr) == 1 && flr != 0){
                elevator_request[flr][1] = 1;
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
    return NULL;
}

void initialize_system(){
    printf("Initializing...\n");
    elevator_reset_floor();
    for (int flr = 0; flr < N_FLOORS; flr++) {        
        for(int btn = 0; btn < 2; btn++){ 
            elevator_request[flr][btn] = 0;
        }
        elevator.call[flr] = 0;
    }
    elevator.state = IDLE;
    elevator.floor = elev_get_floor_sensor_signal();
    elevator.direction = UP;
}

void elevatorControl() {
    initialize_system();
    pthread_t button_monitor_t;
    pthread_create(&button_monitor_t,NULL,button_monitor,"Processing...");
    Elevator_State prev_state = elevator.state;
    int prev_floor = elevator.floor;
    int state_iterator = 1;
    d_print_state(&state_iterator);
    while (!elev_get_stop_signal()) {
        if (elevator.state != prev_state) {
            d_print_state(&state_iterator);
            prev_state = elevator.state;
        }
        switch (elevator.state) {
            case IDLE:
            //current floor call or request in same direction
            if (elevator_request[elevator.floor][elevator.direction] || elevator.call[elevator.floor]) {
                elevator.state = DOORS_OPEN;
                elevator.call[elevator.floor] = 0;
                elevator_request[elevator.floor][elevator.direction] = 0;
                sleep(1);
            }
            else if (elevator_should_advance(elevator, elevator_request)) {
                prev_floor = elevator.floor;
                elevator_move(elevator);
                elevator.state = MOVING;
            }
            else
                elevator.direction = !elevator.direction;
            break;
            case MOVING:
            if (elevator.floor != prev_floor && elevator_should_stop(elevator, elevator_request)) {
                elevator_stop();
                elevator.state = DOORS_OPEN;
                elevator.call[elevator.floor] = 0;
                elevator_request[elevator.floor][elevator.direction] = 0;
                sleep(1);
            }
            break;
            case DOORS_OPEN:
            if (elevator.call[elevator.floor] || elevator_request[elevator.floor][elevator.direction]) {
                elevator.call[elevator.floor] = 0;
                elevator_request[elevator.floor][elevator.direction] = 0;
                sleep(1);
            }
            else if (elevator_should_advance(elevator, elevator_request)) {
                prev_floor = elevator.floor;
                elevator_move(elevator);
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
}

int main(){
    elev_init(ET_Comedi); // ET_Comedi or ET_Simulation
    elev_set_motor_direction(DIRN_STOP);
    elevatorControl();
    elev_set_motor_direction(DIRN_STOP);
}
