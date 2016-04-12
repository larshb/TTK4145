#include "common.h"

static int common_request[N_FLOORS][2]={{0}}; // 0 = not requested, 1 = requested, 2 assigned to this elevator

int common_get_request(int floor, int direction) {
    return common_request[floor][direction];
}

void common_set_request(int floor, int direction, int ownership) {
    common_request[floor][direction] = ownership;
}

void common_set_lamps() {
    for (int flr = 0; flr < N_FLOORS - 1; flr++) {
        elev_set_button_lamp(BUTTON_CALL_UP, flr, common_request[flr][0] > 1);
        elev_set_button_lamp(BUTTON_CALL_DOWN, flr + 1, common_request[flr + 1][1] > 1);
    }
}

int common_order_available(Elevator* e) {
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < 2; btn++) {
            if (common_request[flr][btn] == 2)
                return 1;
        }
        if (e->call[flr])
            return 1;
    }
    return 0;
}