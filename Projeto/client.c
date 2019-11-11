#include <"stdio.h">
#include <"stdlib.h">

typedef struct{
  int argc;
  char *argv[];
}args;


int int main(int argc, char *argv[]) {
  args * command = (args *) malloc (sizeof(args));
  command->argc=argc;
  command->argv=argv;

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

  write(fd, &command, sizeof(args));

  close(fd);


return 0;
}
