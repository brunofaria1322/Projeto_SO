#include "header.h"

commands * verify (int argc, char argv[][MAX], commands * head){
	if (argc > 0){
		char * com = command(argc, argv);
		if (strcmp(argv[0],"DEPARTURE")==0){
			if(argc == 6){
				if (strcmp(argv[2],"init:")==0 && atoi(argv[3]) !=0 && strcmp(argv[4],"takeoff:")==0 && atoi(argv[5]) != 0){
					if (atoi(argv[3]) < mem->t || atoi(argv[5])<atoi(argv[3])){
						printf("Error: Inserted times are invalid.");
						char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
						strcpy(wcom,"Wrong command => "); strcat(wcom,com);
						writeLog(f,wcom);
					}
					else{
						char * com = command(6, argv);
						char * wcom = (char*)malloc(strlen(com)*sizeof(char)+15);
						strcpy(wcom,"New command => "); strcat(wcom,com);
						writeLog(f,wcom);

						Departure *d = (Departure*)malloc(sizeof(Departure));
						strcpy(d->code, argv[2]);
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
				}
				else{

						printf("Error: 'initial time' and 'takeoff time' must be floats- DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}");


					//char * com = command(6, argv);
					char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
					strcpy(wcom,"Wrong command => "); strcat(wcom,com);
					writeLog(f,wcom);
				}
			}
			else{

					printf("Invalid number of arguments (%d). Command takes 6 arguments - DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}",argc);

				char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
				strcpy(wcom,"Wrong command => "); strcat(wcom,com);
				writeLog(f,wcom);
			}
		}
		else if (strcmp(argv[0],"ARRIVAL")==0){
			//char * com = command(argc, argv);
			if(argc == 8){
				if (strcmp(argv[2],"init:")==0 && atoi(argv[3]) !=0 && strcmp(argv[4],"eta:")==0 && atoi(argv[5]) != 0 && strcmp(argv[6],"fuel:") ==0 && atoi(argv[7]) != 0){
					if (atoi(argv[3])<mem->t){
						printf("Error: Inserted init time is invalid.");
						char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
						strcpy(wcom,"Wrong command => "); strcat(wcom,com);
						writeLog(f,wcom);
					}
					else{
						//char * com = command(9, argv);
						char * wcom = (char*)malloc(strlen(com)*sizeof(char)+15);
						strcpy(wcom,"New command => "); strcat(wcom,com);
						writeLog(f,wcom);

						Arrival *a = (Arrival*)malloc(sizeof(Arrival));
						strcpy(a->code, argv[1]);
						a->init = atoi(argv[3]);
						a->eta = atoi(argv[5]);
						a->fuel = atoi(argv[7]);
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
				}

				else{

						printf("Error: 'initial time', 'time to runway' and 'initial fuel' must be floats - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}");


					//char * com = command(9, argv);
					char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
					strcpy(wcom,"Wrong command => "); strcat(wcom,com);
					writeLog(f,wcom);
				}
			}
			else{

				printf("Invalid number of arguments (%d). Command takes 8 arguments - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}",argc);

			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(f,wcom);
			}
		}
		else{

				printf("Unknown command (%s). Available commands are DEPARTURE and ARRIVAL.",argv[0]);

			//char * com = command(argc, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(f,wcom);
		}
	}
	return head;
}
