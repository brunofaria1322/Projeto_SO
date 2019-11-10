#include "header.h"

void pipew(char state, char*argv[]){
	int fd;
	if ((fd = open(PIPE_NAME, O_RDWR)) < 0) { // O_RDONLY  só para leitura, WRONLY só para escrita
		perror("Cannot open pipe for writing: ");
		exit(1);
	}
	if (state == 'd'){
		if (strcmp(argv[3],"init:")==0 && atoi(argv[4]) !=0 && strcmp(argv[5],"takeoff:")==0 && atoi(argv[6]) != 0){
			char * com = command(7, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+15);
			strcpy(wcom,"New command => "); strcat(wcom,com);
			writeLog(wcom);
			Departure *d = (Departure*)malloc(sizeof(Departure));
			d->code = argv[2];
			d->init = atoi(argv[4]);
			d->takeoff = atoi(argv[6]);
			write(fd, &d, sizeof(Departure));
		}
		else{
			#ifdef DEBUG
				printf("Error: 'initial time' and 'takeoff time' must be floats- DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}");
			#endif

			char * com = command(7, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(wcom);
		}
	}
	else if (state == 'a'){
		if (strcmp(argv[3],"init:")==0 && atoi(argv[4]) !=0 && strcmp(argv[5],"eta:")==0 && atoi(argv[6]) != 0 && strcmp(argv[7],"fuel") ==0 && atoi(argv[8]) != 0){
			char * com = command(9, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+15);
			strcpy(wcom,"New command => "); strcat(wcom,com);
			writeLog(wcom);
			Arrival *a = (Arrival*)malloc(sizeof(Arrival));
			a->code = argv[2];
			a->init = atoi(argv[4]);
			a->eta = atoi(argv[6]);
			a->fuel = atoi(argv[8]);
			write(fd, &a, sizeof(Departure));
		}
		else{
			#ifdef DEBUG
				printf("Error: 'initial time', 'time to runway' and 'initial fuel' must be floats - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}");
			#endif

			char * com = command(9, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(wcom);
		}
	}
	else{
		perror("Wrong state value at pipew. state shoud be 'd' for depart or 'a' for arrival.");
		exit(1);
		
	}

}
