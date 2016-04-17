#include "filebackup.h"
#include "common.h"			//access to common_request[N_FLOORS][2]={{0}}; and get, set
#include "elev.h"			//N_FLOORS
#include "elevator.h"		//access to call[N_FLOORS];

#include <stdio.h>			//file I/O
#include <strings.h>		//bzero()
#include <assert.h>


void writeTolog(Elevator* e) {
	FILE *fp;
	char line[255];
	bzero(line,255);
	fp = fopen("log/log.txt", "w");
	for(int flr = 0; flr< N_FLOORS; flr++){
		line[flr] = e->call[flr] + '0';
	}
	line[N_FLOORS] = 'c'; //Add c(complete) for checksum
	fprintf(fp, "%s", line);

	fclose(fp);
}



void readLog(Elevator* e){
	FILE *fp;
	char line[255];
	bzero(line,255);
	fp = fopen("log/log.txt", "r");
	fscanf(fp,"%s",line);
	assert(line[N_FLOORS] == 'c'); //Debug
	//if(line[N_FLOORS] == 'c'){ //check if file is corrupt
	for(int flr = 0; flr < N_FLOORS; flr++){
		e->call[flr] = line[flr] - '0';
	}
	//}
	fclose(fp);
}