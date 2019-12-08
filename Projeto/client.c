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
#define DEBUG
#define MAX 256

int main(int argc, char *argv[]) {

  int i;
  #ifdef DEBUG
    for (i=0;i<argc;i++){
      printf ("Arg[%d] - %s\n",i,argv[i]);
    }

  // Openning the named pipe
    printf("Openning named pipe\n");
  #endif

  int fd;
  char buff[MAX];
  if ((fd = open(PIPE_NAME, O_WRONLY)) < 0) { // O_RDONLY  só para leitura, O_WRONLY só para escrita, O_RDWR para escrita e leitura
    perror("Cannot open pipe for writing: ");
    exit(0);
  }

  if (argc>=2){
    strcat(buff,argv[1]);
    for (i=2;i<argc;i++){
      strcat(buff," ");
      strcat(buff,argv[i]);
    }
  }

  #ifdef DEBUG
    printf ("Sending %s\n",buff);
  #endif

  write(fd,buff,sizeof(buff));


  usleep(100);
  close(fd);
  return 0;
}
