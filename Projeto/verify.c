#include "header.h"

commands * verify (int argc, char argv[][MAX], commands * head){
	if (argc > 0){
		if (strcmp(argv[1],"DEPARTURE")==0){
			char * com = command(argc, argv);
			if(argc == 7){
				if (strcmp(argv[3],"init:")==0 && atoi(argv[4]) !=0 && strcmp(argv[5],"takeoff:")==0 && atoi(argv[6]) != 0){
					char * com = command(7, argv);
					char * wcom = (char*)malloc(strlen(com)*sizeof(char)+15);
					strcpy(wcom,"New command => "); strcat(wcom,com);
					writeLog(f,wcom);

					Departure *d = (Departure*)malloc(sizeof(Departure));
					d->code = argv[2];
					d->init = atoi(argv[4]);
					d->takeoff = atoi(argv[6]);
					commands * comm= malloc(sizeof(commands));
					comm->dep=d;
					comm->arr=NULL;
					comm->init=d->init;

					#ifdef DEBUG
			      printf ("adicionando o comando %s\n",wcom);
			    #endif

					head =addCommand(comm, head);


					#ifdef DEBUG
						 printf ("Comando adicionado com init: %d\n", head->init);
					 #endif

				}
				else{
					#ifdef DEBUG
						printf("Error: 'initial time' and 'takeoff time' must be floats- DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}");
					#endif

					char * com = command(7, argv);
					char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
					strcpy(wcom,"Wrong command => "); strcat(wcom,com);
					writeLog(f,wcom);
				}
			}
			else{
				#ifdef DEBUG
					printf("Invalid number of arguments (%d). Command takes 6 arguments - DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}",argc);
				#endif
				char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
				strcpy(wcom,"Wrong command => "); strcat(wcom,com);
				writeLog(f,wcom);
			}
		}
		else if (strcmp(argv[1],"ARRIVAL")==0){
			char * com = command(argc, argv);
			if(argc == 9){
				if (strcmp(argv[3],"init:")==0 && atoi(argv[4]) !=0 && strcmp(argv[5],"eta:")==0 && atoi(argv[6]) != 0 && strcmp(argv[7],"fuel") ==0 && atoi(argv[8]) != 0){
					char * com = command(9, argv);
					char * wcom = (char*)malloc(strlen(com)*sizeof(char)+15);
					strcpy(wcom,"New command => "); strcat(wcom,com);
					writeLog(f,wcom);

					Arrival *a = (Arrival*)malloc(sizeof(Arrival));
					a->code = argv[2];
					a->init = atoi(argv[4]);
					a->eta = atoi(argv[6]);
					a->fuel = atoi(argv[8]);
					commands * comm= malloc(sizeof(commands));
					comm->arr=a;
					comm->dep=NULL;
					comm->init=a->init;

			    #ifdef DEBUG
			      printf ("adicionando o comando %s\n",wcom);
			    #endif

					head =addCommand(comm, head);

					#ifdef DEBUG
					 printf ("Comando adicionado\n");
				 #endif
					}

				else{
					#ifdef DEBUG
						printf("Error: 'initial time', 'time to runway' and 'initial fuel' must be floats - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}");
					#endif

					char * com = command(9, argv);
					char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
					strcpy(wcom,"Wrong command => "); strcat(wcom,com);
					writeLog(f,wcom);
				}
			}
			else{
			#ifdef DEBUG
				printf("Invalid number of arguments (%d). Command takes 8 arguments - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}",argc);
			#endif
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(f,wcom);
			}
		}
		else{
			#ifdef DEBUG
				printf("Unknown command (%s). Available commands are DEPARTURE and ARRIVAL.",argv[1]);
			#endif
			char * com = command(argc, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(f,wcom);
		}
	}
	return head;
}
