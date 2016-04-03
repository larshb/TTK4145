//debug.c



void d_goto_floor(int lvl) {
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
}