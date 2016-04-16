#pragma once

#include "elev.h" // N_FLOORS

void tcp_client_init(const char* master_ip);
int tcp_common_call(char button, char action, int floor); /* action: 'c' = completed, 'r' = requested */
int tcp_get_station_rank();
const char* tcp_get_next_master_ip();
void tcp_update_status(int state, int direction, int floor);
void tcp_get_common_requests(int new_common_request[N_FLOORS][2]);
const char* tcp_client_send(char instruction[255]);
void tcp_client_kill();
void tcp_test();