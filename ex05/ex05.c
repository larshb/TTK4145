//Needs the driver package to run

#include "elev.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    elev_init();

    printf("Press STOP button to stop elevator and exit program.\n");

    /*
    //elev_set_motor_direction()
    while(1) {
        elev_set_motor_direction(DIRN_DOWN);
        sleep(1);
        elev_set_motor_direction(DIRN_UP);
        sleep(1);
        elev_set_motor_direction(DIRN_STOP);
        sleep(1);
    }
    */

    /*
    //elev_set_button_lamp()
    while(1) {
        for(int flr = 0; flr < 4; flr++){
            for(int btn = 0; btn < 3; btn++){
                elev_set_button_lamp(btn, flr, 1);
                sleep(1);
                elev_set_button_lamp(btn, flr, 0);
            }
        }
    }
    */

    /*
    //elev_set_floor_indicator()
    while(1) {
        for(int flr = 0; flr < 4; flr++) {
            elev_set_floor_indicator(flr);
            sleep(1);
            elev_set_floor_indicator(flr);
        }
    }
    */

    /*
    //elev_set_door_open_lamp()
    while(1){
        elev_set_door_open_lamp(1);
        sleep(1);
        elev_set_door_open_lamp(0);
        sleep(1);
    }
    */

    /*
    //elev_get_button_signal()
    int change = 0;
    int curr[4][3] = {0};
    int prev[4][3] = {0};
    while (1) {
    for(int flr = 0; flr <4; flr++){
        for(int btn = 0; btn < 3; btn++){
            curr[flr][btn] = elev_get_button_signal(btn, flr);
            if (curr[flr][btn] != prev[flr][btn]){
                prev[flr][btn] = curr[flr][btn];
                change = 1;
            }
        }
    }
    if (change){
        for(int flr = 0; flr <4; flr++){
            for(int btn = 0; btn < 3; btn++){
                printf("%i  ", prev[flr][btn]);
            }
            printf("\n");
        }
        printf("\n");
        change = 0;
    }
    */

    // Stop elevator and exit program if the stop button is pressed
    if (elev_get_stop_signal()) {
        elev_set_motor_direction(DIRN_STOP);
        return 0;
    }
    

}
