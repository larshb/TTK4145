#include "elev.h"
#include "elevator.c"

#include <stdio.h>
#include <pthread.h> // POSIX threads
#include <semaphore.h> // for what?
#include <string.h>

//Debug
//#include "debug.c"
#include <unistd.h> // sleep

/*typedef enum { OFF, ON } ES_Lamp_State;

typedef struct {
    int             current_floor;
    ES_Lamp_State   button[N_FLOORS][N_BUTTONS];
    ES_Lamp_State   stop;
    ES_Lamp_State   door_open;
} ES_Panel_Lamps;*/

static Elevator elevator;
//static ES_Panel_Lamps lamps;
static int stop_Button = 0;
static int elevator_request[N_FLOORS][2];
//static int elevator_call[N_FLOORS]; //debug only
//static int prev_elevator_calls[N_FLOORS][N_BUTTONS];

pthread_mutex_t lock1;
pthread_mutex_t lock2;

void panel_set_lamps() {
    for (int flr = 0; flr < N_FLOORS - 1; flr++) {
        elev_set_button_lamp(DIRN_UP, flr, elevator_request[flr][0]);
        elev_set_button_lamp(DIRN_DOWN, flr + 1, elevator_request[flr + 1][1]);
    }
}

int order_available(Elevator e) {
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < 2; btn++) {
            if (elevator_request[flr][btn])
                return 1;
        }
        if (e.call[flr])
            return 1;
    }
    return 0;
}

int should_stop(Elevator e) {
    if (e.floor == 0 || e.floor == N_FLOORS - 1 || e.call[e.floor] || elevator_request[e.floor][e.direction])
        return 1;
    return 0;
}

int should_advance(Elevator e) {
    if (e.direction == DOWN && e.floor > 0) {
        for (int flr = e.floor - 1; flr > -1; flr--){
            if(e.call[flr] || elevator_request[flr][DOWN]){
                return 1;
            }
        }
    }
    else if (e.direction == UP && e.floor < N_FLOORS - 1) {
        for (int flr = e.floor + 1; flr < N_FLOORS; flr++){
            if(e.call[flr] || elevator_request[flr][UP]){
                return 1;
            }
        }
    }
    return 0;
}

void d_print_state() {
    printf("\n[F]loor [U]p [D]own [C]ommand\n\n");
    printf("+---+-------+\n| F | U D C |\n+---+-------+\n");
    for (int flr = 3; flr != -1; flr--) {
        printf("| %d |", flr + 1);
        for (int btn = 0; btn < 2; btn++) {
            if (elevator_request[flr][btn] == 1)
                printf(" #");
            else
                printf("  ");
        }
        if (elevator.call[flr])
            printf(" #");
        else
            printf("  ");
        printf(" |\n");
    }
    printf("+---+-------+\n");
    printf("Elevator floor: %d\n", elevator.floor + 1);
    printf("Elevator state: %d\n", elevator.state);
}

void *button_monitor(){
    while(!stop_Button) {
        for (int flr = 0; flr < N_FLOORS; flr++){
            if( elev_get_button_signal(BUTTON_CALL_UP,flr) == 1 && flr != N_FLOORS -1){
                elevator_request[flr][0] = 1;
            }
            if( elev_get_button_signal(BUTTON_CALL_DOWN,flr) == 1 && flr != 0){
                elevator_request[flr][1] = 1;
            }
            if( elev_get_button_signal(BUTTON_COMMAND,flr) == 1){
                elevator.call[flr] = 1;
            }
        }
        /*for (int flr = 0; flr < N_FLOORS; flr++){
            for (int btn = 0; btn < N_BUTTONS; btn++){
                lamps.button[flr][btn] = elevator_calls[flr][btn];
            }
        }
        _setLamps(lamps);*/
        if(elev_get_stop_signal()){
            stop_Button = 1;
        }
    }
    return 0;
}

/*void _setLamps(ES_Panel_Lamps lamps) {
    elev_set_floor_indicator(lamps.current_floor);
    elev_set_door_open_lamp(lamps.door_open);
    elev_set_stop_lamp(lamps.stop);
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < N_BUTTONS; btn++) 
            elev_set_button_lamp(btn, flr, lamps.button[flr][btn]);
        // may implement not setting non-existent lamps
    }
}*/

/*
void printToLog(){
    FILE *logFile;
    logFile = fopen("/home/student/Desktop/logFile.txt", "a");
    fprintf(logFile, "[F]loor [U]p [D]own [C]ommand\n");
    fprintf(logFile, "+---+-------+\n| F | U D C |\n+---+-------+\n");
    for (int flr = 3; flr != -1; flr--) {
        fprintf(logFile, "| %d |", flr + 1);
        for (int btn = 0; btn < 3; btn++) {
            if (elevator_calls[flr][btn] == 1)
                fprintf(logFile, " #");
            else
                fprintf(logFile, "  ");
        }
        fprintf(logFile, " |\n");
    }
    fprintf(logFile, "+---+-------+\n");
    fprintf(logFile, "Elevator floor: %d\n", elevator.floor);
    fprintf(logFile, "Elevator state: %d\n", elevator.state);

    fclose(logFile);
}

void *logMonitor(){
     //Reset Matrix
    for(int flr = 0; flr < N_FLOORS; flr++){        
            for(int btn = 0; btn < N_BUTTONS; btn++){ 
                prev_elevator_calls[flr][btn] = 0;
            }
        }
    int stop = 0;
    while(!stop_Button){
        stop = 0;
        for(int flr = 0; flr < N_FLOORS && !stop; flr++){
            for(int btn = 0; btn < N_BUTTONS && !stop; btn++){
                if(prev_elevator_calls[flr][btn] != elevator_calls[flr][btn]){
                    stop = 1;
                    for(int flr = 0; flr < N_FLOORS; flr++){
                        for(int btn = 0; btn < N_BUTTONS; btn++){
                            prev_elevator_calls[flr][btn] = elevator_calls[flr][btn];            
                        }
                     }
                     d_print_state();
                     printToLog();
                }
            }
        }
    }
}
*/

void *elevatorState(){
    /*
    ES_Panel_Lamps lamps;
    lamps.current_floor = 0;
    lamps.stop = OFF;
    lamps.door_open = OFF;
    for (int flr = 0; flr < N_FLOORS; flr++){
        lamps.button[flr][2] = OFF;
    }
    while(1) {
        for (int flr = 0; flr < N_FLOORS; flr++){
            if (elev_get_button_signal(BUTTON_COMMAND, flr))
                elevator_calls[flr][2] = 1;
        } 
        for (int flr = 0; flr < N_FLOORS; flr++){
            lamps.button[flr][2] = elevator_calls[flr][2];
        }
        _setLamps(lamps);
    }
    */
    return 0;
}

/*void threadTest(){
    pthread_t button_monitor; // thread identifier
    //pthread_t ElevatorMonitor; // thread identifier

    pthread_create(&button_monitor,NULL,button_monitor,"Processing..."); // Create worker thread for button_monitor, format = (&thread identifier, attribute argument(This is typically NULL), thread function, argument)
    //pthread_create(&ElevatorMonitor,NULL,elevatorState,"Processing..."); // Create worker thread for elevatorState

    elevator_reset_floor();
    while(1){
        for (int flr = 0; flr < N_FLOORS; flr++){
            for(int btn = 0; btn < N_BUTTONS; btn++){
                if(elevator_calls[flr][btn] == 1){
                    d_goto_floor(flr);
                    elevator_calls[flr][0] = 0;
                    elevator_calls[flr][1] = 0;
                    elevator_calls[flr][2] = 0;
                }
            }
        d_goto_floor(0);
        d_goto_floor(3);
        
        if (elev_get_stop_signal()){
            elev_set_motor_direction(DIRN_STOP);
            //break;
        }
    }
    pthread_join(button_monitor,NULL);
    //pthread_join(ElevatorMonitor,NULL);

    }
}*/



void initializeSystem(){

    elevator_reset_floor();

    //Reset Matrix
    for (int flr = 0; flr < N_FLOORS; flr++) {        
        for(int btn = 0; btn < 2; btn++){ 
            elevator_request[flr][btn] = 0;
        }
        elevator.call[flr] = 0;
    }

    //Initialize elevator
    elevator.state = IDLE;
    elevator.floor = elev_get_floor_sensor_signal();
    elevator.direction = UP;

    //Initialize lamp system;
    /*for (int flr = 0; flr < N_FLOORS; flr++){        
        for (int btn = 0; btn < N_BUTTONS; btn++){ 
            lamps.button[flr][btn] = OFF;
        }
    }
    lamps.stop = OFF;
    lamps.door_open = OFF;
    lamps.current_floor = elevator.floor;
    _setLamps(lamps);
    */
}

void elevatorControl(){
    initializeSystem(); // Initialize order matrix, elevator_reset_floor(), elevator struct and lamp struct.
    
    //Initialize monitor
    pthread_t button_manager;
    pthread_create(&button_manager,NULL,button_monitor,"Processing...");

    //pthread_t LogMonitor;
    //pthread_create(&LogMonitor,NULL,*logMonitor,"Processing...");
    int state_change = 0;
    panel_set_lamps();
    while (!stop_Button) {
        if (state_change) {
            d_print_state();
            state_change = 0;
        }
        switch (elevator.state) {
            case IDLE:
            if (order_available(elevator)) {
                if (!should_advance(elevator))
                    elevator.direction = !elevator.direction;
                elevator.state = MOVING;
                state_change = 1;
            }
            break;
            case MOVING:
            if (elev_get_floor_sensor_signal() != -1) {
                elevator.floor = elev_get_floor_sensor_signal();
                state_change = 1;
            }
            if (elevator.direction == UP) {
                elev_set_motor_direction(DIRN_UP);
                state_change = 1;
            }
            else {
                elev_set_motor_direction(DIRN_DOWN);
                state_change = 1;
            }
            if (should_stop(elevator)) {
                elev_set_motor_direction(DIRN_STOP);
                elevator.state = DOORS_OPEN;
                state_change = 1;
            }
            break;
            case DOORS_OPEN:
            if (elevator.direction == UP) 
                elevator_request[elevator.floor][0] = 0;
            else
                elevator_request[elevator.floor][1] = 0;
            elevator.call[elevator.floor] = 0;
            sleep(2);
            if (should_advance(elevator)) {
                elevator.state = MOVING;
                if (elevator.direction == UP)
                    elev_set_motor_direction(DIRN_UP);
                else
                    elev_set_motor_direction(DIRN_DOWN);
                printf("\n\nSHOULD MOVE, elevator.direction=%d\n\n", elevator.direction);
            }
            else {
                elevator.state = IDLE;
                printf("\n\nSHOULD NOT MOVE\n\n");
            }
            state_change = 1;
            break;
            case STOPPED:
            //
            break;
        }
    }

    //Starting elevator operation loop
    /*
    while(!stop_Button){
        switch (elevator.state){
            case IDLE: //IDLE STATE
                for(int flr = 0; flr < N_FLOORS; flr++){
                    for (int btn = 0; btn < N_BUTTONS; btn++){ //Go through matrix and search for orders.
                        if (elevator_calls[flr][btn] == 1){
                            if(btn == 2){ //COMMAND-BUTTON
                                if(flr == elevator.floor){
                                    sleep(1);
                                    elevator_calls[elevator.floor][2] = 0;
                                }
                                else if(flr > elevator.floor){
                                    elevator.state = MOVING_UP;
                                }
                                else if(flr < elevator.floor){
                                    elevator.state = MOVING_DOWN;
                                }
                            }
                            else if(btn == 1){//DOWN-BUTTON
                                if(flr == elevator.floor){
                                    elevator_open_door(); //Open door for 2 seconds
                                    elevator.state = IDLE;
                                    elevator_calls[elevator.floor][1] = 0;
                                }
                                else if(flr > elevator.floor){
                                    elevator.state = MOVING_UP;
                                }
                                else if(flr < elevator.floor){
                                    elevator.state = MOVING_DOWN;
                                }
                            }
                            else if(btn == 0){//UP-BUTTON
                                if(flr == elevator.floor){
                                    elevator_open_door(); //Open door for 2 seconds
                                    elevator.state = IDLE;
                                    elevator_calls[elevator.floor][0] = 0;
                                }
                                else if(flr > elevator.floor){
                                    elevator.state = MOVING_UP;
                                }
                                else if(flr < elevator.floor){
                                    elevator.state = MOVING_DOWN;
                                }
                            }
                        }
                    }
                }
                break;
            case MOVING_UP: // UP-STATE
                if(elevator.floor != N_FLOORS - 1){
                    elevator_go_up(); //Goes 1 floor up
                    elevator.floor = elev_get_floor_sensor_signal(); //Update elevator floor;
                    lamps.current_floor = elevator.floor; //Update floor light
                    _setLamps(lamps);
                    if (elevator_calls[elevator.floor][0] == 1 || elevator_calls[elevator.floor][2] == 1){
                        elevator_open_door(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][2] = 0;
                        elevator_calls[elevator.floor][0] = 0;

                        if(requests_above(elevator.floor) == 1){
                            elevator.state = MOVING_UP;
                        }
                        else{
                            elevator.state = IDLE;
                        }
                    }
                    else if (elevator_calls[elevator.floor][1] == 1){
                        elevator_open_door(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][1] = 0;
                        elevator.state = IDLE;
                    }
                }
                else{
                    elevator_open_door(); //Open door for 2 seconds
                }
                        
                break;
            case MOVING_DOWN: //DOWN-STATE
                if(elevator.floor != 0){
                    elevator_go_down();
                    elevator.floor = elev_get_floor_sensor_signal(); //Update elevator floor;
                    lamps.current_floor = elevator.floor; //Update floor light
                    _setLamps(lamps);
                    if (elevator_calls[elevator.floor][1] == 1 || elevator_calls[elevator.floor][2] == 1){
                        elevator_open_door(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][2] = 0;
                        elevator_calls[elevator.floor][1] = 0;
                        if(requests_below(elevator.floor) == 1){
                            elevator.state = MOVING_DOWN;
                        }
                        else{
                            elevator.state = IDLE;
                        }
                    }
                    else if (elevator_calls[elevator.floor][0] == 1){
                        elevator_open_door(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][0] = 0;
                        elevator.state = IDLE;
                    }

                }
                else{
                    elevator_open_door(); //Open door for 2 seconds
                }
                break;
        }
    }*/
    pthread_join(button_manager,NULL); //Kill monitor
    //pthread_join(LogMonitor,NULL); //Kill monitor
}



/*
Choose direction:

    Continue in the current direction of travel if there are any further requests in that direction
    Otherwise, change direction if there are requests in the opposite direction
    Otherwise, stop and become idle

Should stop:

    Stop if there are passengers that want to get off at this floor
    Stop if there is a request in the direction of travel at this floor
    Stop if there are no further requests in this direction
*/


int main(){
    elev_init(ET_Simulation); // ET_Comedi or ET_Simulation
    elev_set_motor_direction(DIRN_STOP);
    //button_monitor();
    elevatorControl();
    //threadTest();
    //elevator_go_down();
}
