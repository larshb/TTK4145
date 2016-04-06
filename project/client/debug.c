#include "debug.h"

#include <stdio.h> // printf

/*void d_goto_floor(int lvl, Elevator* e) {
    if (lvl > elev_get_floor_sensor_signal() && elev_get_floor_sensor_signal() != -1){
        elev_set_motor_direction(DIRN_UP);
        e->state = MOVING;
        e->direction = DIRN_UP;
    }
    else if (lvl < elev_get_floor_sensor_signal()){
        elev_set_motor_direction(DIRN_DOWN);
        e->state = MOVING;
        e->direction = DIRN_DOWN;
        } 
    while (elev_get_floor_sensor_signal() != lvl){
        if (elev_get_stop_signal())
            break;
    }
    elev_set_motor_direction(DIRN_STOP);
}*/

void debug_print_state(int* iteration, Elevator* e, int reqs[N_FLOORS][2]) {
    printf("\n[F]loor [U]p [D]own [C]ommand\n");
    printf("+---+-------+\n| F | U D C |\n+---+-------+\n");
    for (int flr = N_FLOORS - 1; flr > -1; flr--) {
        printf("| %d |", flr);
        for (int btn = 0; btn < 2; btn++) {
            if (reqs[flr][btn] == 1)
                printf(" #");
            else
                printf("  ");
        }
        if (e->call[flr])
            printf(" #");
        else
            printf("  ");
        printf(" |");
        if (flr == e->floor) {
            if (e->state != MOVING) {
                if (e->state == DOORS_OPEN)
                    printf("[ ]");
                else
                    printf("+");
            }
            else {
                if (e->direction == UP)
                    printf("^");
                else
                    printf("v");
            }
        }
        printf("\n");
    }
    printf("+---+-------%d\n", (*iteration)++);
    printf("Elevator floor: %d\n", e->floor);
    printf("Elevator state: %s\n", elevator_state_to_string(e));
}