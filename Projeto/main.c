#include "header.h"

char* command(int argc, char *argv[]){
	int size = 0;
	for(int i=1;i<argc;i++){
		size = size + strlen(argv[i]);
	}
	char * com = (char*)malloc(size*sizeof(char)+1 + argc);
	strcpy(com,argv[1]); strcat(com," ");
	for(int i=2; i<argc;i++){
		strcat(com,argv[i]); strcat(com," ");
	}
	return com;
}

int main(int argc, char *argv[]){

	writeLog("Program started running.");
	Data data;
	data=readConfig(data);
	printData(data);
	pid_t id = fork();
	if (id == 0){
		torre();
		exit(0);
	}
	else if (id <0){
		printf("the creation of a child process was unsuccessful.");
	}
	else{
		wait(NULL);
	}

	// Creates the named pipe if it doesn't exist yet
	#ifdef DEBUG
		printf("Creating named pipe\n");
	#endif

	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
		perror("Cannot create pipe: ");
		exit(1);
	}

	// Create the shared memory segment if it doesn't exist yet
	#ifdef DEBUG
		printf("Creating the shared memory segment\n");
	#endif

	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, sizeof(SharedMemory), IPC_CREAT))< 0) {
		perror("Couldn't get/create the shared memory segment!\n");
		exit(1);
	}

	//TODO: Complete this semaphore part
	// Create an array of 2 semaphores
	#ifdef DEBUG
		printf("Creating an array of 2 semaphores\n");
	#endif

	int semid;
	if ( (semid=semget(IPC_PRIVATE, 2, IPC_CREAT|0777)) == -1 ){
    perror("Could not get the semaphore set!");
    return (1);
  }


	if (argc > 0){
		if (strcmp(argv[1],"DEPARTURE")==0){
			char * com = command(argc, argv);
			if(argc == 7){
				pipew('d',argv);
			}
			else{
				printf("Invalid number of arguments (%d). Command takes 6 arguments - DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}",argc);
				char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
				strcpy(wcom,"Wrong command => "); strcat(wcom,com);
				writeLog(wcom);
			}
		}
		else if (strcmp(argv[1],"ARRIVAL")==0){
			char * com = command(argc, argv);
			if(argc == 9){
				pipew('a',argv);
			}
			else{
			printf("Invalid number of arguments (%d). Command takes 8 arguments - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}",argc);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(wcom);
			}
		}
		else{
			printf("Unknown command (%s). Available commands are DEPARTURE and ARRIVAL.",argv[1]);
			char * com = command(argc, argv);
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(wcom);
		}
	}

writeLog("Program finished running.");
}

/*
void fixInput(char *string){
    string[strlen(string)-1]='\0';
}
*/

void printData(Data data){
  printf("%d\n%d, %d\n%d, %d\n%d, %d\n%d\n%d\n",data.ut,data.T,data.dt,data.L,data.dl,data.min,data.max,data.D,data.A);
	printf("\n");
}
