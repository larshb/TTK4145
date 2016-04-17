#include "filebackup.h"

#include "elevator.h"		// N_FLOORS, Elevator

#include <stdio.h>			// fopen, fscanf, fclose
#include <strings.h>		// bzero
#include <assert.h>			// assert


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
	assert(fp != NULL);
	fscanf(fp,"%s",line);
	assert(line[N_FLOORS] == 'c'); //Debug
	//if(line[N_FLOORS] == 'c'){ //check if file is corrupt
	for(int flr = 0; flr < N_FLOORS; flr++){
		e->call[flr] = line[flr] - '0';
	}
	//}
	fclose(fp);
}