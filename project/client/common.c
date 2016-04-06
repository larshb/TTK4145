#include "common.h"

void common_initialize(int reqs[N_FLOORS][2]) {
    for (int flr = 0; flr < N_FLOORS; flr++) {        
        for(int btn = 0; btn < 2; btn++){ 
            reqs[flr][btn] = 0;
        }
    }
}

void common_set_lamps(int reqs[N_FLOORS][2]) {
    for (int flr = 0; flr < N_FLOORS - 1; flr++) {
        elev_set_button_lamp(BUTTON_CALL_UP, flr, reqs[flr][0]);
        elev_set_button_lamp(BUTTON_CALL_DOWN, flr + 1, reqs[flr + 1][1]);
    }
}

int common_order_available(Elevator* e, int reqs[N_FLOORS][2]) {
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < 2; btn++) {
            if (reqs[flr][btn])
                return 1;
        }
        if (e->call[flr])
            return 1;
    }
    return 0;
}