#pragma once

void tcp_server_init();
void *tcp_server_test();
void *elevator_connection_handler(void *);




//DEBUG/FØRSTEUTKAST/////////////////////////
int add_remote_elevator(int socket);
void delete_remote_elevator(int elevator_id);
////////////////////////////////////////////