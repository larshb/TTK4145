#pragma once

void tcp_server_init();
void *tcp_server_test();
void *elevator_connection_handler(void *);




//DEBUG/FØRSTEUTKAST/////////////////////////
void add_connected_elevator(int socket);
void delete_connected_elevator(int socket);
////////////////////////////////////////////