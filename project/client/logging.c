//logging.c

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