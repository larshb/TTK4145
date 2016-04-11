#pragma once

void tcp_client_init();
void tcp_common_call(char button, char action, int floor); /* action: 'c' = completed, 'r' = requested */
void tcp_client_kill();
void tcp_test();