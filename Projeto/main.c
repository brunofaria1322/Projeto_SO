#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define PIPE_NAME   "input_pipe"
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
	else{wait(NULL);}
	// Creates the named pipe if it doesn't exist yet
	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
	perror("Cannot create pipe: ");
	exit(0);
	}
	// Opens the pipe for reading
	int fd;
	if ((fd = open(PIPE_NAME, O_RDWR)) < 0) { // O_RDONLY  só para leitura, WRONLY só para escrita
	perror("Cannot open pipe for writing: ");
	exit(0);
	}
	// Aqui vai estar a função de verificação
	//numbers n;
	//while (1) {
	//read(fd, &n, sizeof(numbers));
	//printf("[SERVER] Received (%d,%d), adding it: %d\n",
	//n.a, n.b, n.a+n.b);
}


void error(char *erro){
	printf("Error: %s\n",erro);
	exit(-1);
}

void fixInput(char *string){
    string[strlen(string)-1]='\0';
}

void printData(Data data){
  printf("%d\n%d, %d\n%d, %d\n%d, %d\n%d\n%d\n",data.ut,data.T,data.dt,data.L,data.dl,data.min,data.max,data.D,data.A);
	printf("\n");
}
