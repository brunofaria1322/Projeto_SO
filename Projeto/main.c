#include "header.h"

int main(){
	Data data;
	data=readConfig(data);
	printData(data);
	pid_t id = fork();
	if (id == 0){
		torre();
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

	// Opens the pipe for reading
	#ifdef DEBUG
		printf("Openning the pipe for reading\n");
	#endif

	int fd;
	if ((fd = open(PIPE_NAME, O_RDWR)) < 0) { // O_RDONLY  só para leitura, WRONLY só para escrita
		perror("Cannot open pipe for writing: ");
		exit(1);
	}


	// Aqui vai estar a função de verificação
	//numbers n;
	//while (1) {
	//read(fd, &n, sizeof(numbers));
	//printf("[SERVER] Received (%d,%d), adding it: %d\n",
	//n.a, n.b, n.a+n.b);



	// Create the shared memory segment if it doesn't exist yet
	#ifdef DEBUG
		printf("Creating the shared memory segment\n");
	#endif

	int shmid;
	//TODO: Segundo argumento de shmget tem de ser alterado para sizeof(struct da memoria partilhada ainda por criar)
	if ((shmid = shmget(IPC_CREAT, sizeof(data), IPC_CREAT))< 0) {
		perror("Couldn't get/create the shared memory segment!\n");
		exit(1);
	}


	// Create an array of 2 semaphores
	#ifdef DEBUG
		printf("Creating an array of 2 semaphores\n");
	#endif

	int semid;
	if ( (semid=semget(IPC_PRIVATE, 2, IPC_CREAT|0777)) == -1 ){
    perror("Could not get the semafore set!");
    return (1);
  }


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
