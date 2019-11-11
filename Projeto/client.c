#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>


typedef struct{
  int argc;
  char *argv[];
}args;

#define PIPE_NAME   "input_pipe"

int main(int argc, char *argv[]) {
  args * command = (args *) malloc (sizeof(args));
  command->argc=argc;
  int i;
  for (i=0;i<argc;i++){
    command->argv[i]=argv[i];
    printf ("Arg[%d] - %s\n",i,command->argv[i]);
  }

	// Creates the named pipe if it doesn't exist yet
	#ifdef DEBUG
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
  if ((fd = open(PIPE_NAME, O_RDWR)) < 0) { // O_RDONLY  só para leitura, WRONLY só para escrita
    perror("Cannot open pipe for writing: ");
    exit(1);
  }

  write(fd, &command, sizeof(args));

  for (i=0;i<command->argc;i++){
    printf ("Arg[%d] - %s\n",i,command->argv[i]);
  }
  close(fd);
  return 0;
}
