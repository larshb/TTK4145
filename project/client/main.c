#include "elevator.h"
#include "common.h"
#include "timer.h"
#include "tcp_client.h"
#include "tcp_server.h"

#include <stdio.h>      // printf 
#include <pthread.h>    // POSIX threads
#include <semaphore.h>  // for what?
#include <string.h>     // string
#include <unistd.h>     // sleep
#include <stdlib.h>     // system 

// Debug
#include "debug.h"

static Elevator elevator;

void master_main() {
    common_init();
    tcp_server_init();
    tcp_client_init();
    pthread_t common_monitor_t;
    pthread_t elevator_monitor_t;
    pthread_t tcp_server_test_t;
    pthread_create(&common_monitor_t,NULL,common_monitor,"Processing...");
    pthread_create(&elevator_monitor_t,NULL,elevator_monitor,&elevator);
    pthread_create(&tcp_server_test_t,NULL,tcp_server_test,"Processing...");
    Elevator_State prev_state = elevator.state;
    elevator.role = MASTER;
    elevator.rank = tcp_get_station_rank();
    int prev_floor = elevator.floor;

    printf("My rank: %d   My role: %d\n", elevator.rank, elevator.role);
    //int state_iterator = 1;
    //debug_print_state(&state_iterator, &elevator, 1);

    while (!elev_get_stop_signal()) {

        //backup
        // if (timer_timeout(&message_timer)) {
        //     sendMessage(message);
        //     timer_set(&message_timer, 100);
        // }

        if (elevator.state != prev_state) {

            //debug_print_state(&state_iterator, &elevator, 1);
            
            tcp_update_status(elevator.state, elevator.direction, elevator.floor);
            prev_state = elevator.state;
        }
        switch (elevator.state) {
            case IDLE:
            //current floor call or request in same direction
            if (common_get_request(elevator.floor, elevator.direction) == 2 || elevator.call[elevator.floor]) {
                elevator_door_open(&elevator);
                elevator.state = DOORS_OPEN;
                elevator.call[elevator.floor] = 0;
                common_set_request(elevator.floor, elevator.direction, 0);
                char button = elevator.direction ? 'd' : 'u';
                tcp_common_call(button, 'c', elevator.floor);
            }
            else if (elevator_should_advance(&elevator)) {
                prev_floor = elevator.floor;
                elevator_move(&elevator);
                elevator.state = MOVING;
            }
            else
                elevator.direction = !elevator.direction;
            break;
            case MOVING:
            if (elevator.floor != prev_floor && elevator_should_stop(&elevator)) {
                elevator_stop();
                if (common_get_request(elevator.floor, elevator.direction) == 2 || elevator.call[elevator.floor]) {
                    elevator_door_open(&elevator);
                    elevator.state = DOORS_OPEN;
                    elevator.call[elevator.floor] = 0;
                    common_set_request(elevator.floor, elevator.direction, 0);
                    char button = elevator.direction ? 'd' : 'u';
                    tcp_common_call(button, 'c', elevator.floor);
                }
                else
                    elevator.state = IDLE;
            } 
            break;
            case DOORS_OPEN:
            if (elevator_door_closed(&elevator)) {

                // Open door on request
                if (elevator.call[elevator.floor] || common_get_request(elevator.floor, elevator.direction) == 2) {
                    elevator.call[elevator.floor] = 0;
                    common_set_request(elevator.floor, elevator.direction, 0);
                    char button = elevator.direction ? 'd' : 'u';
                    tcp_common_call(button, 'c', elevator.floor);
                    elevator_door_open(&elevator);
                }

                // Continue to next floor
                else if (elevator_should_advance(&elevator)) {
                    prev_floor = elevator.floor;
                    elevator_move(&elevator);
                    elevator.state = MOVING;
                }

                else {
                    // Continue opposite direction
                    elevator.direction = !elevator.direction;
                    if (elevator_should_advance(&elevator)) {
                        elevator_move(&elevator);
                        elevator.state = MOVING;
                    }

                    // Go to idle
                    else
                        elevator.state = IDLE;
                }
            }
            break;
            case STOPPED:
            break;
        }
    }
    tcp_client_kill();
    pthread_join(common_monitor_t,NULL); // Kill monitor
    pthread_join(elevator_monitor_t,NULL);
    pthread_join(tcp_server_test_t, NULL);
    elev_set_motor_direction(DIRN_STOP);
    common_complete();
}

void _print_help() {
    printf("  -m\t\trun as master\n");
    printf("  -s\t\trun as slave\n");
}

int main(int argc, char* argv[]){
    if (argc != 2) {
        _print_help();
    }
    else {
        if (argv[1][0] == '-') {
            switch (argv[1][1]) {
                case 'm':
                master_main();
                break;
                case 's':
                tcp_client_init();
                int a = tcp_get_station_rank();
                printf("I have rank: %d\n", a);
                //common_monitor();
                break;
                default:
                _print_help();
                break;
            }
        }
        else
            _print_help();
    }
    while(1);
    return 0;






    /*
    if (argc != 2) {
        printHelp();
    }
    else {
        if (argv[1][0] == '-') {
            switch (argv[1][1]) {
                case 'm':
                system("gnome-terminal -e \"./elevator -b\"");
                master_main();
                break;
                case 'b':
                runBackup();
                system("clear & gnome-terminal -e \"./elevator -b\"");
                master_main();
                //master_main();
                break;
                default:
                printHelp();
                break;
            }
        }
        else
            printHelp();
    }
    return 0;
    */
}
