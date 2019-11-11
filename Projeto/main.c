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

	commands * head=(commands *) malloc(sizeof(commands));
	head=NULL;

	info * inf;
	inf = (info *)malloc(sizeof(info));
	inf->ut=data.ut;
	inf->head=head;

	pthread_t timer;
	pthread_create (&timer, NULL,(void *)ftimer,(void *) inf);

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

	/*
	// Create an array of 2 threads
	#ifdef DEBUG
		printf("Creating an array of 2 Threads\n");
	#endif

	pthread_t flights[2];					//flights[0]== DEPARTURE //flights[1]== Arrival

	pthread_create ( flights[0], NULL, fDep, )
	pthread_create ()
	*/

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

	args * command = (args *) malloc(sizeof(args));
	int fd;

	printf("Vou pro while\n");
	while (1){

		if ((fd = open(PIPE_NAME, O_RDWR)) >= 0) { // O_RDONLY  só para leitura, WRONLY só para escrita

			//ler do client
			read(fd,&command,sizeof(command));
			//recebe bem
			printf ("%d aaaaaaaaaa\n",command->argc);
			int i;
			for (i=0;i<command->argc;i++){
				printf ("Arg[%d] - %s\n",i,command->argv[i]);
			}
			head=verifica(command->argc, command->argv, head);
		}

	}


writeLog("Program finished running.");
}

/*
void fixInput(char *string){
    string[strlen(string)-1]='\0';
}
*/

//TODO: Juntar verifica com verify
commands * verifica (int argc, char *argv[], commands * head){
	if (argc > 0){
		if (strcmp(argv[1],"DEPARTURE")==0){
			char * com = command(argc, argv);
			if(argc == 7){
				verify('d',argv, head);
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
				verify('a',argv,head);
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
	return head;
}

void printData(Data data){
  printf("%d\n%d, %d\n%d, %d\n%d, %d\n%d\n%d\n",data.ut,data.T,data.dt,data.L,data.dl,data.min,data.max,data.D,data.A);
	printf("\n");
}

void ftimer(void * infor){
	info * inf= (info *) infor;
	int ut= inf->ut;
	printf("UT %d\n",ut);
	commands * head = inf->head;
	int t=0, ti=1000*ut;
	while (1){
		if(head){
			while (t==head->init) {
				printf("Tou no while com head_init %d\n",t);
				if (head->arr!=NULL){
					pthread_t arriv;
					pthread_create(&arriv,NULL,(void *)fArrival,head->arr);
				}
				else {
					pthread_t depar;
					pthread_create(&depar,NULL,(void *)fDepart,head->dep);
				}
				head=removeFirstCommand(head);
			}
		}
		usleep (ti);
		t++;
	}
}

commands* removeFirstCommand(commands * head){
  // Store head node
  commands* temp = head;
  head = temp->next;   			// Changed head
  free(temp);               // free old head
  return head;
}

commands* addCommand(commands * node, commands * head){
	commands *tmp, *ant;
  if (head==NULL)
      head=node;

  else{

      if (head->init>node->init) {
          node->next=head;
          head=node;
      }
      else {
          ant=head;
          tmp=head->next;
          while ((tmp!=NULL) && (tmp->init<node->init)) {
              ant=tmp;
              tmp=tmp->next;
          }

          node->next=tmp;
          ant->next=node;
      }
		}
  return(head);
}

void fDepart(Departure * departure){
	char buf[MAX];
	sprintf(buf,"New departure %s on Thread %d",departure->code,(int ) pthread_self());
	writeLog(buf);

	//Continue
}

void fArrival(Arrival * arrival){
	char buf[MAX];
	sprintf(buf,"New Arrival %s on Thread %d",arrival->code,(int ) pthread_self());
	writeLog(buf);

	//continue
}
/*
float getTime(int ut){
	return (((clock()/CLOCK_PER_SEC)*1000)/ut)
}
*/
