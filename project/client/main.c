#include <stdio.h>

#include <unistd.h> // sleep

#include <pthread.h> //POSIX threads

#include <semaphore.h>

#include "elev.h"

#include <string.h>





typedef enum {
    OFF     = 0,
    ON      = 1
} ES_Lamp_State;

typedef struct {
    int             current_floor;
    ES_Lamp_State   button[N_FLOORS][N_BUTTONS];
    ES_Lamp_State   stop;
    ES_Lamp_State   door_open;
} ES_Panel_Lamps;

typedef enum {
    IDLE,
    //DOORS_OPEN,
    MOVING_UP,
    MOVING_DOWN
    //STOPPED
} Elevator_State;

typedef enum { false, true } bool;

typedef struct {
    int                     floor;
    elev_motor_direction_t  direction;
    Elevator_State          state;
} Elevator;


static Elevator elevator;
static ES_Panel_Lamps lamps;
static bool stop_Button = false;
static int elevator_calls[N_FLOORS][N_BUTTONS];
static int prev_elevator_calls[N_FLOORS][N_BUTTONS];

const int LOG_SIZE = 100;

pthread_mutex_t lock1;
pthread_mutex_t lock2;

void setLamps(ES_Panel_Lamps lamps) {
    elev_set_floor_indicator(lamps.current_floor);
    elev_set_door_open_lamp(lamps.door_open);
    elev_set_stop_lamp(lamps.stop);
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for (int btn = 0; btn < N_BUTTONS; btn++) 
            elev_set_button_lamp(btn, flr, lamps.button[flr][btn]);
    }
}

/*void gotoFloor(int lvl) {
    if (lvl > elev_get_floor_sensor_signal() && elev_get_floor_sensor_signal() != -1){
        elev_set_motor_direction(DIRN_UP);
        elevator.state = MOVING;
        elevator.direction = DIRN_UP;
    }
    else if (lvl < elev_get_floor_sensor_signal()){
        elev_set_motor_direction(DIRN_DOWN);
        elevator.state = MOVING;
        elevator.direction = DIRN_DOWN;
        } 
    while (elev_get_floor_sensor_signal() != lvl){
        if (elev_get_stop_signal())
            break;
    }
    elev_set_motor_direction(DIRN_STOP);
}*/

void findFloor() {
	if (elev_get_floor_sensor_signal() == -1){
		elev_set_motor_direction(DIRN_DOWN);
		while(elev_get_floor_sensor_signal() == -1);
		elev_set_motor_direction(DIRN_STOP);
	}
}

int _callPressed() {
	for (int i = 0; i < 4; i++){
		if (elev_get_button_signal(2, i))
			return i;
	}
	return -1;
}

void printState() {
	printf("[F]loor [U]p [D]own [C]ommand\n");
	printf("+---+-------+\n| F | U D C |\n+---+-------+\n");
	for (int flr = 3; flr != -1; flr--) {
		printf("| %d |", flr + 1);
		for (int btn = 0; btn < 3; btn++) {
			if (elevator_calls[flr][btn] == 1)
				printf(" #");
			else
				printf("  ");
		}
		printf(" |\n");
	}
	printf("+---+-------+\n");
    printf("Elevator floor: %d\n", elevator.floor);
    printf("Elevator state: %d\n", elevator.state);
}

/*void _moveTest() {
	int prevFloor = -1;
	int currFloor = -1;
	while(1){
        findFloor();
		currFloor = _callPressed();
		if (currFloor != -1 && currFloor != prevFloor){
			printState();
			gotoFloor(currFloor);
            prevFloor = currFloor;
		}
	}
}

void _disco() {
    ES_Panel_Lamps lamps;
    ES_Lamp_State common_state = OFF;
    lamps.current_floor = 0;
    while(1) {
        lamps.current_floor = (lamps.current_floor + 1) % N_FLOORS;
        common_state = lamps.current_floor % 2;
        for (int flr = 0; flr < N_FLOORS; flr++) {
            for (int btn = 0; btn < N_BUTTONS; btn++)
                lamps.button[flr][btn] = common_state;
        }
        lamps.stop = common_state;
        lamps.door_open = common_state;
        setLamps(lamps);
        sleep(1);
    }
}*/



void goDown(){
    findFloor();
    int currFlr = elev_get_floor_sensor_signal(); 
    if (elev_get_floor_sensor_signal() > 0){
        elev_set_motor_direction(DIRN_DOWN);
        elevator.direction = DIRN_DOWN; 
        while (elev_get_floor_sensor_signal() != currFlr - 1){}
        elev_set_motor_direction(DIRN_STOP);
        elevator.direction = DIRN_STOP;
    }
}

void goUp(){
    findFloor();
    int currFlr = elev_get_floor_sensor_signal(); 
    if (elev_get_floor_sensor_signal() < 3){
        elev_set_motor_direction(DIRN_UP);
        elevator.direction = DIRN_UP;
        while (elev_get_floor_sensor_signal() != currFlr + 1){}
        elev_set_motor_direction(DIRN_STOP);
        elevator.direction = DIRN_STOP;
    }

}

void *systemState(){
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for(int btn = 0; btn < N_BUTTONS; btn ++){
            elevator_calls[flr][btn] = 0;
        }
        for (int btn = 0; btn < N_BUTTONS; btn++){
            lamps.button[flr][btn] = OFF;
        }
    }
    while(!stop_Button) {
        for (int flr = 0; flr < N_FLOORS; flr++){
            if( elev_get_button_signal(BUTTON_CALL_UP,flr) == 1 && flr != N_FLOORS -1){
                elevator_calls[flr][BUTTON_CALL_UP] = 1;
            }
            if( elev_get_button_signal(BUTTON_CALL_DOWN,flr) == 1 && flr != 0){
                elevator_calls[flr][BUTTON_CALL_DOWN] = 1;
            }
            if( elev_get_button_signal(BUTTON_COMMAND,flr) == 1){
                elevator_calls[flr][BUTTON_COMMAND] = 1;
            }
        }
        for (int flr = 0; flr < N_FLOORS; flr++){
            for (int btn = 0; btn < N_BUTTONS; btn++){
                lamps.button[flr][btn] = elevator_calls[flr][btn];
            }
        }
        setLamps(lamps);
        if(elev_get_stop_signal()){
            stop_Button = true;
        }
    }
    
}

void printToLog(){
    FILE *logFile;
    logFile = fopen("/home/student/Desktop/logFile.txt" , "a");

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
                     printState();
                     printToLog();
                }
            }
        }
    }
}



void *elevatorState(){
    ES_Panel_Lamps lamps;
    lamps.current_floor = 0;
    lamps.stop = OFF;
    lamps.door_open = OFF;
    for (int flr = 0; flr < N_FLOORS; flr++){
        elevator_calls[flr][2] = 0;
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
        setLamps(lamps);
    }

}

/*void threadTest(){
    pthread_t SystemMonitor; // thread identifier
    //pthread_t ElevatorMonitor; // thread identifier

    pthread_create(&SystemMonitor,NULL,systemState,"Processing..."); // Create worker thread for systemState, format = (&thread identifier, attribute argument(This is typically NULL), thread function, argument)
    //pthread_create(&ElevatorMonitor,NULL,elevatorState,"Processing..."); // Create worker thread for elevatorState

    findFloor();
    while(1){
        for (int flr = 0; flr < N_FLOORS; flr++){
            for(int btn = 0; btn < N_BUTTONS; btn++){
                if(elevator_calls[flr][btn] == 1){
                    gotoFloor(flr);
                    elevator_calls[flr][0] = 0;
                    elevator_calls[flr][1] = 0;
                    elevator_calls[flr][2] = 0;
                }
            }
        gotoFloor(0);
        gotoFloor(3);
        
        if (elev_get_stop_signal()){
            elev_set_motor_direction(DIRN_STOP);
            //break;
        }
    }
    pthread_join(SystemMonitor,NULL);
    //pthread_join(ElevatorMonitor,NULL);

    }
}*/

void openDoor(){
    lamps.door_open = ON;
    setLamps(lamps);
    sleep(2); //Door open for 2 seconds;
    lamps.door_open = OFF;
    setLamps(lamps);
}


int checkAbove(int fl){
    for (int flr = fl + 1; flr < N_FLOORS; flr++ ){
        if(elevator_calls[flr][0] == 1 || elevator_calls[flr][2] == 1){
            return 1;
        }
    }
    return 0;
}

int checkBelow(int fl){
    for (int flr = fl - 1; flr > -1; flr-- ){
        if(elevator_calls[flr][1] == 1 || elevator_calls[flr][2] == 1){
            return 1;
        }
    }
    return 0;
}

void initializeSystem(){

    findFloor();

    //Reset Matrix
    for(int flr = 0; flr < N_FLOORS; flr++){        
            for(int btn = 0; btn < N_BUTTONS; btn++){ 
                elevator_calls[flr][btn] = 0;
            }
        }

    //Initialize elevator
    elevator.state = IDLE;
    elevator.direction = DIRN_STOP;
    elevator.floor = elev_get_floor_sensor_signal();

    //Initialize lamp system;
    for(int flr = 0; flr < N_FLOORS; flr++){        
            for(int btn = 0; btn < N_BUTTONS; btn++){ 
                lamps.button[flr][btn] = OFF;
            }
        }
    lamps.stop = OFF;
    lamps.door_open = OFF;
    lamps.current_floor = elevator.floor;
    setLamps(lamps);

}

void elevatorControl(){
    initializeSystem(); // Initialize order matrix, findFloor(), elevator struct and lamp struct.
    
    //Initialize monitor
    pthread_t SystemMonitor;
    pthread_create(&SystemMonitor,NULL,systemState,"Processing...");

    pthread_t LogMonitor;
    pthread_create(&LogMonitor,NULL,*logMonitor,"Processing...");

    //Starting elevator operation loop
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
                            else if(btn == 1){//DOWN-BUTTOn
                                if(flr == elevator.floor){
                                    openDoor(); //Open door for 2 seconds
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
                                    openDoor(); //Open door for 2 seconds
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
                    goUp(); //Goes 1 floor up
                    elevator.floor = elev_get_floor_sensor_signal(); //Update elevator floor;
                    lamps.current_floor = elevator.floor; //Update floor light
                    setLamps(lamps);
                    if (elevator_calls[elevator.floor][0] == 1 || elevator_calls[elevator.floor][2] == 1){
                        openDoor(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][2] = 0;
                        elevator_calls[elevator.floor][0] = 0;

                        if(checkAbove(elevator.floor) == 1){
                            elevator.state = MOVING_UP;
                        }
                        else{
                            elevator.state = IDLE;
                        }
                    }
                    else if (elevator_calls[elevator.floor][1] == 1){
                        openDoor(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][1] = 0;
                        elevator.state = IDLE;
                    }
                }
                else{
                    openDoor(); //Open door for 2 seconds
                }
                        
                break;
            case MOVING_DOWN: //DOWN-STATE
                if(elevator.floor != 0){
                    goDown();
                    elevator.floor = elev_get_floor_sensor_signal(); //Update elevator floor;
                    lamps.current_floor = elevator.floor; //Update floor light
                    setLamps(lamps);
                    if (elevator_calls[elevator.floor][1] == 1 || elevator_calls[elevator.floor][2] == 1){
                        openDoor(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][2] = 0;
                        elevator_calls[elevator.floor][1] = 0;
                        if(checkBelow(elevator.floor) == 1){
                            elevator.state = MOVING_DOWN;
                        }
                        else{
                            elevator.state = IDLE;
                        }
                    }
                    else if (elevator_calls[elevator.floor][0] == 1){
                        openDoor(); //Open door for 2 seconds
                        elevator_calls[elevator.floor][0] = 0;
                        elevator.state = IDLE;
                    }

                }
                else{
                    openDoor(); //Open door for 2 seconds
                }
                break;
        }
    }
    pthread_join(SystemMonitor,NULL); //Kill monitor
    pthread_join(LogMonitor,NULL); //Kill monitor
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
    elev_init(ET_Comedi); // ET_Comedi or ET_Simulation
    //systemState();
    elevatorControl();
    //threadTest();
    //goDown();
}
