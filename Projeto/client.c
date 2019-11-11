#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


#define PIPE_NAME   "input_pipe"


int main(int argc, char *argv[]) {

  int i;
  #ifdef DEBUG
    for (i=0;i<argc;i++){
      printf ("Arg[%d] - %s\n",i,argv[i]);
    }

	// Creates the named pipe if it doesn't exist yet
	
		printf("Creating named pipe\n");
	#endif

	if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
		perror("Cannot create pipe: ");
		exit(1);
	}

  // Openning the named pipe
  #ifdef DEBUG
    printf("Openning named pipe\n");
  #endif

  int fd;
  if ((fd = open(PIPE_NAME, O_RDWR)) < 0) { // O_RDONLY  só para leitura, O_WRONLY só para escrita, O_RDWR para escrita e leitura
    perror("Cannot open pipe for writing: ");
    exit(1);
  }

  write(fd,&argc, sizeof(argc));

  for (i=0;i<argc;i++){
    #ifdef DEBUG
      printf ("Sending Arg[%d] - %s\n",i,argv[i]);
    #endif
    write(fd,argv[i],strlen(argv[i])+1);
    usleep(100);      //should be semaphore
  }
  close(fd);
  return 0;
}
