#include "header.h"

int main(int argc, char *argv[]){

	writeLog("Program started running.");
	Data data;
	data=readConfig(data);
	
	#ifdef DEBUG
		printData(data);
	#endif

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

	int fd,i,num;
	char buff[MAX];
	char args[32][MAX];

	while (1){

		if ((fd = open(PIPE_NAME,  O_RDWR)) >= 0) { // O_RDONLY  só para leitura, O_WRONLY só para escrita, O_RDWR para escrita e leitura

			//ler do client
			read(fd,&num,sizeof(num));


		  for (i=0;i<num;i++){
	    	read(fd,buff, sizeof(buff));
				strcpy(args[i],buff);
		  }

			//recebe bem
			#ifdef DEBUG
				printf ("Recieved %d args\n",num);
				for (i=0;i<num;i++){
					printf ("Arg[%d] - %s\n",i,args[i]);
				}
			#endif

			close(fd);
			inf->head=verifica(num, args, inf->head);
			#ifdef DEBUG
				printf ("head->init: %d\ni",inf->head->init);
			#endif
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
commands * verifica (int argc, char argv[][MAX], commands * head){
	if (argc > 0){
		if (strcmp(argv[1],"DEPARTURE")==0){
			char * com = command(argc, argv);
			if(argc == 7){
				head=verify('d',argv, head);
			}
			else{
				#ifdef DEBUG
					printf("Invalid number of arguments (%d). Command takes 6 arguments - DEPARTURE {flight_code} init: {initial time} takeoff: {takeoff time}",argc);
				#endif
				char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
				strcpy(wcom,"Wrong command => "); strcat(wcom,com);
				writeLog(wcom);
			}
		}
		else if (strcmp(argv[1],"ARRIVAL")==0){
			char * com = command(argc, argv);
			if(argc == 9){
				head=verify('a',argv,head);
			}
			else{
			#ifdef DEBUG
				printf("Invalid number of arguments (%d). Command takes 8 arguments - ARRIVAL {flight_code} init: {initial time} eta: {time to runway} fuel: {initial fuel}",argc);
			#endif
			char * wcom = (char*)malloc(strlen(com)*sizeof(char)+16);
			strcpy(wcom,"Wrong command => "); strcat(wcom,com);
			writeLog(wcom);
			}
		}
		else{
			#ifdef DEBUG
				printf("Unknown command (%s). Available commands are DEPARTURE and ARRIVAL.",argv[1]);
			#endif
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

void ftimer(info * inf){
	//info * inf= (info *) infor;
	#ifdef DEBUG
		printf("UT %d\n",inf->ut);
	#endif

	int t=0, ti=1000*inf->ut;
	while (1){
		#ifdef DEBUG
			printf("Time: %d\n",t);
		#endif
		if(inf->head!=NULL){
			#ifdef DEBUG
				printf("tou no while... existe head t=%d, init=%d\n",t,inf->head->init);
			#endif
			while (inf->head!=NULL && t==inf->head->init) {
				#ifdef DEBUG
					printf("Tou no while com head_init %d\n",t);
				#endif
				if (inf->head->arr!=NULL){
					#ifdef DEBUG
						printf("ARRIVAL\n");
					#endif
					pthread_t arriv;
					pthread_create(&arriv,NULL,(void *)fArrival,inf->head->arr);
					//usleep (ti);
				}
				else {
					#ifdef DEBUG
						printf("DEPARTURE\n");
					#endif
					pthread_t depar;
					pthread_create(&depar,NULL,(void *)fDepart,inf->head->dep);
					//usleep (ti);
				}
				#ifdef DEBUG
					printf("removendo primeiro\n");
				#endif
				inf->head=removeFirstCommand(inf->head);
				#ifdef DEBUG
					printf("removido\n");
				#enif
			}
			//sleep(2);		//remove
			//usleep (ti);
		}
		usleep (ti);
		t++;
	}
}

commands* removeFirstCommand(commands * head){
  // Store head node
	#ifdef DEBUG
		printf("Tou na remove\n");
	#endif
  commands* temp = head;
	if (temp->next!=NULL){
		#ifdef DEBUG
			printf("mais que 1 elem na remove\n");
		#endif
  	head = temp->next;   			// Changed head
		free(temp);               // free old head
	}
	else{
		#ifdef DEBUG
			printf("1 elem na remove\n");
		#endif
		head=NULL;
	}
	#ifdef DEBUG
		printf("sair da remove\n");
	#endif
  return head;
}

commands* addCommand(commands * node, commands * head){
	commands *tmp, *ant;
  if (head==NULL){
			#ifdef DEBUG
				printf("Head NULL\n" );
			#endif
			head=node;
		}
  else{

      if (head->init>node->init) {
					#ifdef DEBUG
						printf("init 1 < init 2\n" );
					#endif
          node->next=head;
          head=node;
      }
      else {
					#ifdef DEBUG
						printf("init 1 > init 2\n" );
					#endif
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

void *fDepart(Departure * departure){
	#ifdef DEBUG
		printf("Thread na Departur\n");
	#endif
	char buf[MAX];
	sprintf(buf,"New departure %s on Thread %d",departure->code,(int ) pthread_self());
	writeLog(buf);

	//Continue
	#ifdef DEBUG
		printf("saida da Thread na Departur\n");
	#endif
	pthread_exit(NULL);
	return NULL;
}

void *fArrival(Arrival * arrival){
	#ifdef DEBUG
		printf("Thread na Arrival\n");
	#endif
	char buf[MAX];
	sprintf(buf,"New Arrival %s on Thread %d",arrival->code,(int ) pthread_self());
	writeLog(buf);

	//continue
	#ifdef DEBUG
		printf("saida da Thread na Arrival\n");
	#endif
	pthread_exit(NULL);
	return NULL;
}


char* command(int argc, char argv[][MAX]){
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

/*
float getTime(int ut){
	return (((clock()/CLOCK_PER_SEC)*1000)/ut)
}
*/
