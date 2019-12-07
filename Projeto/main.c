#include "header.h"

int main(int argc, char *argv[]){

	//Initializing

	//Variables

	int fd,i,num;
	char buff[MAX];
	char args[32][MAX];
	pthread_t timer;
	queue_size = 0;
	#ifdef DEBUG
		printf("Opening file log.txt\n");
	#endif

	if ((f= fopen("log.txt" ,"a+")) == NULL){
			perror("Couldn't open the file log.txt");
			exit(0);
	}

	// Creates the named pipe if it doesn't exist yet
	#ifdef DEBUG
		printf("Creating named pipe\n");
	#endif

	unlink(PIPE_NAME);
	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
		perror("Cannot create pipe");
		exit(0);
	}

	// Create the shared memory segment if it doesn't exist yet
	#ifdef DEBUG
	printf("Creating the shared memory segment\n");
	#endif

	if ((shmid = shmget(IPC_PRIVATE, sizeof(SharedMemory), IPC_CREAT|0600))< 0) {
		perror("Couldn't get/create the shared memory segment!\n");
		exit(0);
	}

	mem = (SharedMemory*) shmat(shmid, NULL, 0);

	data=readConfig(data);

	#ifdef DEBUG
	printData(data);
	#endif

	mem->flights_created = 0;
	mem->flights_landed = 0;
	mem->time2land = 0;
	mem->flights_takingoff=0;
	mem->time2takeoff = 0;
	mem->hm = 0;
	mem->hm_emergency = 0;
	mem->flights_redirected = 0;
	mem->flights_rejected = 0;

	commands * head=(commands *) malloc(sizeof(commands));
	head=NULL;

	info * inf;
	inf = (info *)malloc(sizeof(info));
	inf->ut=data.ut;
	inf->head=head;

	//create semaphores
  sem_unlink(SEMLOG);
  sem_unlink(SEMSHM);

  semLog = sem_open(SEMLOG, O_CREAT|O_EXCL, 0600, 1);
  semShM = sem_open(SEMSHM, O_CREAT|O_EXCL, 0600, 1);

	// // Create an array of 2 semaphores
	// #ifdef DEBUG
	// printf("Creating an array of 2 semaphores\n");
	// #endif
	//
	// if ( (semid=semget(IPC_PRIVATE, 2, IPC_CREAT|0600)) == -1 ){
  //   perror("Could not get the semaphore set!");
  //   exit(0);
  // }


	// Create the message queue
	#ifdef DEBUG
	printf("Creating message queue.\n");
	#endif

	if((mqid = msgget(IPC_PRIVATE, IPC_CREAT|0700))==-1){
	   	perror("creating message queue.");
	    exit(0);
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

	//Signals
	signal (SIGINT,sigint);
	signal (SIGUSR1,showStats);

	//main
	writeLog(f, "Program started running.");
	pthread_create (&timer, NULL,(void *)ftimer,(void *) inf);

	pid = fork();
	if (pid == 0){
		tower();
		exit(0);
	}
	else if (pid <0){
		perror("the creation of a child process was unsuccessful.");
		exit(0);
	}
	int total,n;

	while (1){

		if ((fd = open(PIPE_NAME,  O_RDWR)) >= 0) { // O_RDONLY  só para leitura, O_WRONLY só para escrita, O_RDWR para escrita e leitura

			//ler do client
			read(fd,&num,sizeof(num));


		  for (i=0;i<num;i++){
				n=total=0;
				while (total < MAX) {
					n =  read(fd, (char*)buff + total,sizeof(buff)-total);
					total+= n;
				}
				strcpy(args[i],buff);
		  }

			close(fd);

			//recebe bem
			#ifdef DEBUG
				printf ("Recieved %d args\n",num);
				for (i=0;i<num;i++){
					printf ("Arg[%d] - %s\n",i,args[i]);
				}
			#endif

			inf->head=verify(num, args, inf->head);
			#ifdef DEBUG
				if (inf->head) printf ("head->init: %d\ni",inf->head->init);
			#endif
		}
	}
}

	/*<
	void fixInput(char *string){
	    string[strlen(string)-1]='\0';
	}
	*/

//Exit signal
void sigint (int signum){
	//Terminating ad Closing everything
	if(pid!=0){
		wait(NULL);												//wait for child to terminate
		writeLog(f,"Program finished running.");
		unlink(PIPE_NAME);								//unlink linked pipe
		fclose(f);												//close log file
		sem_close(semLog);									//closes log semaphore
		sem_close(semShM);									//closes SharedMemory semaphore
		unlink(SEMLOG);										//unlink Log semaphore
		unlink(SEMSHM);										//unlink SharedMemory semaphore
		//semctl(semid, 0, IPC_RMID);			//releases semaphore
		shmctl(shmid, IPC_RMID, NULL);		//releases shared memory
		msgctl(mqid, IPC_RMID, NULL);			//releases message queue
		exit(0);
	}
	else{
	printf("Control Tower terminated\n");
	exit(0);
	}
}

void showStats (int signum){
	sem_wait(semShM);

	printf("Total number of flights created: %d\nTotal number of flights that landed: %d\nAverage wait time (beyond ETA) to land: %d\nTotal number of flights that took off: %d\nAverage wait time to take off: %d\nAverage number of holding maneuvers per landing flight: %d\nAverage number of holding maneuvers per emergency flight: %d\nNumber of flights redirected to another airport: %d\nFlights rejected by the Control Tower: %d\n",mem->flights_created,mem->flights_landed,mem->time2land,mem->flights_takingoff,mem->time2takeoff,mem->hm,mem->hm_emergency,mem->flights_redirected,mem->flights_rejected);

	sem_post(semShM);
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

	t=0; int ti=1000*inf->ut;
	while (1){
		if(t%30==0){
			printf("Time: %d\n",t);
		}
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
				}
				else {
					#ifdef DEBUG
						printf("DEPARTURE\n");
					#endif
					pthread_t depar;
					pthread_create(&depar,NULL,(void *)fDepart,inf->head->dep);
				}
				#ifdef DEBUG
					printf("removendo primeiro\n");
				#endif
				inf->head=removeFirstCommand(inf->head);
				#ifdef DEBUG
					printf("removido\n");
				#endif
			}
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
	queue_size++;
	#ifdef DEBUG
		printf("Thread na Departur\n");
	#endif
	char buf[MAX];
	sprintf(buf,"New departure %s on Thread %d",departure->code,(int ) pthread_self());
	writeLog(f,buf);
	Msg_deparr msgd;
	Msg_slot msgs;
	msgd.mtype = 1;
	msgd.dep=*(departure);
	msgsnd(mqid, &msgd, sizeof(msgd), 0);
	//Continue
	#ifdef DEBUG
		printf("saida da Thread na Departur\n");
	#endif
	msgrcv(mqid, &msgs, sizeof(msgs), 3, 0);
	printf("slot = %d\n", msgs.slot);
	queue_size--;
	pthread_exit(NULL);
	return NULL;

	while(1){
		if(strcmp(mem->slots[msgs.slot],BYEBYE)==0){
			queue_size--;
			pthread_exit(NULL);
			return NULL;
		}
	}
}

void *fArrival(Arrival * arrival){
	queue_size++;
	#ifdef DEBUG
		printf("Thread na Arrival\n");
	#endif
	char buf[MAX];
	sprintf(buf,"New Arrival %s on Thread %d",arrival->code,(int ) pthread_self());
	writeLog(f,buf);
	Msg_deparr msgd;
	Msg_slot msgs;
	msgd.mtype = 2;
	msgd.arr=*(arrival);
	if (msgd.arr.fuel<msgd.arr.eta+data.L+4){
		msgd.arr.emer = 1;
	}
	else{msgd.arr.emer = 0;}
	msgsnd(mqid, &msgd , sizeof(msgd), 0);
	//continue
	#ifdef DEBUG
		printf("saida da Thread na Arrival\n");
	#endif
	//int slot;
	msgrcv(mqid, &msgs, sizeof(msgs), 3, 0);
	printf("slot = %d",msgs.slot);
	queue_size--;
	pthread_exit(NULL);
	return NULL;
	while(1){
		if(strcmp(mem->slots[msgs.slot],BYEBYE)==0){
			queue_size--;
			pthread_exit(NULL);
			return NULL;
		}
	}
	queue_size--;
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
