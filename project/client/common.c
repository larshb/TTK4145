#include "common.h"

#include "tcp_client.h"
#include "pthread.h"    // pthread_mutex_t
#include "timer.h"      // common_request TCP polling

#include <string.h>     // strlen

//debug
//#include "debug.h"
#include "stdio.h"      // printf
#include "unistd.h"     // sleep

#define POLLING_CYCLE 500

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

/*void common_set_next_master_ip((*(const char*))master_ip_str_ptr, const char* new_ip) {
    printf("set: %s\n", new_ip);
    if (strcmp(new_ip, "") != 0) {// && !strcmp(new_ip, common_get_next_master_ip())) {
        pthread_mutex_lock(&common_request_lock);
        next_master_ip = new_ip;
        pthread_mutex_unlock(&common_request_lock);
        printf("Set new master ip to \"%s\"\n", common_get_next_master_ip());
    }
}*/

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
    int new_common_request[N_FLOORS][2];
    struct timeval polling_timer;
    timer_set(&polling_timer, POLLING_CYCLE); // maybe change?
    while (!elev_get_obstruction_signal()) {
        common_set_lamps();
        if (timer_timeout(&polling_timer)) {
            timer_set(&polling_timer, POLLING_CYCLE);
            //tcp_get_common_requests(new_common_request);
            tcp_get_common_requests(new_common_request);
            
            // Update next master IP
            //pthread_mutex_lock(&common_request_lock);
            //const char* new_next_master_ip = tcp_get_next_master_ip();
            //printf("new_next_master_ip = \"%s\"\n", new_next_master_ip);
            //printf("(int)strlen(new_next_master_ip) = %i\n", (int)strlen(new_next_master_ip));
            //printf("!strcmp(new_next_master_ip, \"\" = %d)\n", !strcmp(new_next_master_ip, ""));
            //common_set_next_master_ip(&next_master_ip, new_next_master_ip);
            //printf("right-after: %s\n", common_get_next_master_ip()); 
            //pthread_mutex_unlock(&common_request_lock);
            //sleep(1);

            //debug
            //printf("------Next master IP: %s\n", next_master_ip);

            pthread_mutex_lock(&common_request_lock);
            for (int flr = N_FLOORS - 1; flr > -1; flr--) {
                common_request[flr][0] = new_common_request[flr][0];
                common_request[flr][1] = new_common_request[flr][1];
            }
            pthread_mutex_unlock(&common_request_lock);
        }
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