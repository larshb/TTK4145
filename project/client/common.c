#include "common.h"
#include "tcp_client.h"
#include "pthread.h"    // pthread_mutex_t
#include "stdio.h"      // puts (debug)

static int common_request[N_FLOORS][2]={{0}}; // 0 = not requested, 1 = requested, 2 assigned to this elevator

pthread_mutex_t common_request_lock;

void common_init() {
    pthread_mutex_init(&common_request_lock, NULL);
}

void common_complete() {
    pthread_mutex_destroy(&common_request_lock);
}

int common_get_request(int floor, int direction) {
    pthread_mutex_lock(&common_request_lock);
    int val = common_request[floor][direction];
    pthread_mutex_unlock(&common_request_lock);
    return val;
}

void common_set_request(int floor, int direction, int ownership) {
    pthread_mutex_lock(&common_request_lock);
    common_request[floor][direction] = ownership;
    pthread_mutex_unlock(&common_request_lock);
}

void common_set_lamps() {
    for (int flr = 0; flr < N_FLOORS - 1; flr++) {
        elev_set_button_lamp(BUTTON_CALL_UP, flr, common_get_request(flr, 0) > 1);
        elev_set_button_lamp(BUTTON_CALL_DOWN, flr + 1, common_get_request(flr + 1, 1) > 1);
    }
}

int common_order_available(Elevator* e) {
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < 2; btn++) {
            if (common_get_request(flr, btn) == e->rank)
                return 1;
        }
        if (e->call[flr])
            return 1;
    }
    return 0;
}

void* common_monitor() {
    while (!elev_get_obstruction_signal()) {
        common_set_lamps();
        for (int flr = 0; flr < N_FLOORS; flr++){
            if (elev_get_button_signal(BUTTON_CALL_UP,flr) == 1 && flr != N_FLOORS -1){
                if (!common_get_request(flr, 0)) {
                    common_set_request(flr, 0, 1);
                    common_set_request(flr, 0, tcp_common_call('u', 'r', flr));
                }
            }
            if (elev_get_button_signal(BUTTON_CALL_DOWN,flr) == 1 && flr != 0){
                if (!common_get_request(flr, 1)) {
                    common_set_request(flr, 1, 1);
                    common_set_request(flr, 1, tcp_common_call('d', 'r', flr));
                }
            }
        }
    }
    return 0;
}