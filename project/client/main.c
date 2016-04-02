#include <stdio.h>

#include <unistd.h> // sleep

#include <pthread.h> //POSIX threads

#include <semaphore.h>

#include "elev.h"





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
    MOVING_DOWN,
    STOPPED
} Elevator_State;

typedef enum { false, true } bool;

typedef struct {
    int                     floor;
    elev_motor_direction_t  direction;
    Elevator_State          state;
} Elevator;


static Elevator elevator;
static int elevator_calls[N_FLOORS][N_BUTTONS];

//Initialized mutex locks
//pthread_mutex_t lock1;
//pthread_mutex_t lock2;

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
	for (int flr = 0; flr < N_FLOORS; flr++){
        for(int btn = 0; N_BUTTONS < 3; btn++){
            if (elevator_calls[flr][btn] == 1){
                return flr;
            }
        }
	}
	return -1;
}

void printState() {
	printf("[F]loor [U]p [D]own [C]ommand\n");
	printf("+---+-------+\n| F | U D C |\n+---+-------+\n");
	for (int flr = 3; flr != -1; flr--) {
		printf("| %d |", flr + 1);
		for (int btn = 0; btn < 3; btn++) {
			if (elevator_calls[flr][btn])
				printf(" #");
			else
				printf("  ");
		}
		printf(" |\n");
	}
	printf("+---+-------+\n");
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

void *_monitor(){
    ES_Panel_Lamps lamps;
    lamps.current_floor = 0;
    lamps.stop = OFF;
    lamps.door_open = OFF; 
    for (int flr = 0; flr < N_FLOORS; flr++) {
        for(int btn = 0; btn < N_BUTTONS; btn ++){
            elevator_calls[flr][btn] = 0;
        }
        for (int btn = 0; btn < N_BUTTONS; btn++){
            lamps.button[flr][btn] = OFF;
        }
    }
    while(!elev_get_stop_signal()) {
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
        for(int flr = 0; flr < N_FLOORS; flr++){
            for(int btn = 0; btn < N_BUTTONS; btn++){
                lamps.button[flr][btn] = elevator_calls[flr][btn];
            }
        }
        setLamps(lamps);
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

<<<<<<< HEAD
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
=======
void threadTest(){
    pthread_t SystemMonitor; // thread identifier
    //pthread_t ElevatorMonitor; // thread identifier

    pthread_create(&SystemMonitor,NULL,_monitor,NULL); // Create worker thread for _monitor, format = (&thread identifier, attribute argument(This is typically NULL), thread function, argument)
    //pthread_create(&ElevatorMonitor,NULL,elevatorState,"Processing..."); // Create worker thread for elevatorState

    findFloor();
    while(!elev_get_stop_signal()){
        for (int flr = 0; flr < N_FLOORS; flr++){
            for(int btn = 0; btn < N_BUTTONS; btn++){
                if(elevator_calls[flr][btn]){
                    printState();
>>>>>>> 701349f0c8f7e4dc352ffcc7a1d2330a7355c09f
                    gotoFloor(flr);
                }
                elevator_calls[flr][btn] = OFF;
            }
<<<<<<< HEAD
        gotoFloor(0);
        gotoFloor(3);
        
        if (elev_get_stop_signal()){
            elev_set_motor_direction(DIRN_STOP);
            //break;
        }
    }
    pthread_join(SystemMonitor,NULL);
=======
        }
    }
    elev_set_motor_direction(DIRN_STOP);
    pthread_exit(NULL);
    //pthread_join(SystemMonitor,NULL);
>>>>>>> 701349f0c8f7e4dc352ffcc7a1d2330a7355c09f
    //pthread_join(ElevatorMonitor,NULL);

    }
}*/


void openDoor(){
    findFloor();
    elev_set_door_open_lamp(10);
    sleep(1);
    elev_set_door_open_lamp(0); // Denne er kanskje ingen vits med tanke på at dette kan styres lampesystemet. Men bevarer til enkel testing.

}

void upState(){

}

void downState(){

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
        if(elevator_calls[flr][0] == 1 || elevator_calls[flr][2] == 1){
            return 1;
        }
    }
    return 0;
}

<<<<<<< HEAD
void elevatorControl(){
    findFloor();
    //Initialize monitor
    pthread_t SystemMonitor;
    pthread_create(&SystemMonitor,NULL,systemState,"Processing...");

    //Condition for the elevator to run
    bool stop_Button = false;

    //Initialize elevator
    Elevator elevator;
    elevator.state = IDLE;
    elevator.direction = DIRN_STOP;
    elevator.floor = elev_get_floor_sensor_signal();

    //Initialize lamp system;
    ES_Panel_Lamps lamps;
    for(int flr = 0; flr < N_FLOORS; flr++){        
            for(int btn = 0; btn < N_BUTTONS; btn++){ 
                lamps.button[flr][btn] = OFF;
            }
        }
    lamps.stop = OFF;
    lamps.door_open = OFF;
    lamps.current_floor = elevator.floor;
    setLamps(lamps);


    //Starting elevator operation loop
    while(!stop_Button){
        if(elev_get_stop_signal()){
            elevator.state = STOPPED;
        }
        switch (elevator.state){
            case IDLE: //IDLE STATE
                for(int flr = 0; flr < N_FLOORS; flr++){
                    for (int btn = 0; btn < N_BUTTONS; btn++){ //Go through matrix and search for orders.
                        if(btn == 2){ //Command
                            if(flr == elevator.floor){}
                            else if(flr > elevator.floor){
                                elevator.state = MOVING_UP;
                            }
                            else if(flr < elevator.floor){
                                elevator.state = MOVING_DOWN;
                            }
                        }
                        else if(btn == 1){//Down
                            if(flr == elevator.floor){
                                //Open door
                                lamps.door_open = ON;
                                setLamps(lamps);
                                sleep(2); //Door open for 2 seconds;
                                lamps.door_open = OFF;
                                setLamps(lamps);

                                elevator.state = MOVING_DOWN;

                            }
                            else if(flr > elevator.floor){
                                elevator.state = MOVING_UP;
                            }
                            else if(flr < elevator.floor){
                                elevator.state = MOVING_DOWN;
                            }
                        }
                        else if(btn == 0){//Up
                              if(flr == elevator.floor){
                                //Open door
                                lamps.door_open = ON;
                                setLamps(lamps);
                                sleep(2); //Door open for 2 seconds;
                                lamps.door_open = OFF;
                                setLamps(lamps);

                                elevator.state = MOVING_UP;

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
                break;
            case MOVING_UP: // Down
                if(elevator.floor != N_FLOORS - 1){
                    goUp();
                    //Update elevator floor;
                    elevator.floor = elev_get_floor_sensor_signal();

                    //Update light
                    lamps.current_floor = elevator.floor;
                    setLamps(lamps);

                    if (elevator_calls[elevator.floor][0] == 1 || elevator_calls[elevator.floor][2] == 1){
                        //Open door
                        lamps.door_open = ON;
                        setLamps(lamps);
                        sleep(2); //Door open for 2 seconds;
                        lamps.door_open = OFF;
                        setLamps(lamps);

                        if(checkAbove(elevator.floor) == 1){
                            elevator.state = MOVING_UP;
                        }
                        else{
                            elevator.state = IDLE;
                        }
                    }

                }
                 else{
                     //Open door
                    lamps.door_open = ON;
                    setLamps(lamps);
                    sleep(2); //Door open for 2 seconds;
                    lamps.door_open = OFF;
                    setLamps(lamps);
                }
                        
                break;
            case MOVING_DOWN: //Up
                if(elevator.floor != 0){
                    goDown();
                    //Update elevator floor;
                    elevator.floor = elev_get_floor_sensor_signal();

                    //Update light
                    lamps.current_floor = elevator.floor;
                    setLamps(lamps);

                    if (elevator_calls[elevator.floor][0] == 1 || elevator_calls[elevator.floor][2] == 1){
                        //Open door
                        lamps.door_open = ON;
                        setLamps(lamps);
                        sleep(2); //Door open for 2 seconds;
                        lamps.door_open = OFF;
                        setLamps(lamps);

                        if(checkBelow(elevator.floor) == 1){
                            elevator.state = MOVING_DOWN;
                        }
                        else{
                            elevator.state = IDLE;
                        }
                    }

                }
                else{
                     //Open door
                    lamps.door_open = ON;
                    setLamps(lamps);
                    sleep(2); //Door open for 2 seconds;
                    lamps.door_open = OFF;
                    setLamps(lamps);
                }
                break;
            case STOPPED:
                stop_Button = true;
                break;
        }
    }

    //Kill monitor
    pthread_join(SystemMonitor,NULL);
}
=======
>>>>>>> 701349f0c8f7e4dc352ffcc7a1d2330a7355c09f

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


<<<<<<< HEAD
int main(){
    elev_init(ET_Simulation); // ET_Comedi or ET_Simulation
    //systemState();
    elevatorControl();
    //threadTest();
    //goDown();
    //goDown();
    //goDown();
=======
void initialize(){ //putte alt som må skje i "initialize" fasen her.
    
    findFloor();
}



void elevatorControl(){
    //Enable system monitor
    pthread_t SystemMonitor; // thread identifier
    pthread_create(&SystemMonitor,NULL,_monitor,"Processing...");


    while(1){

        if(_callPressed()){
            printState();
        }
    }

    //Disable system monitor
    pthread_join(SystemMonitor,NULL);

}



int main() {
    elev_init(ET_Comedi);
    threadTest();
    //_monitor();
    return 0;
>>>>>>> 701349f0c8f7e4dc352ffcc7a1d2330a7355c09f
}