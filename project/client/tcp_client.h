#pragma once

void tcp_client_init();
void tcp_common_call(char button, char action, int floor); /* action: 'c' = completed, 'r' = requested */
int tcp_get_station_rank();
void tcp_update_status(int state, int direction, int floor);
const char* tcp_client_send(char instruction[255]);
void tcp_client_kill();
void tcp_test();