#include "header.h"

void writeLog(FILE *f, char *log){

  if (f == NULL){
			perror("Couldn't write the file log.txt cause its not oppened yet");
			exit(1);
	}

  char buff[64];
  time_t t = time(0);
  struct tm *tmp = gmtime(&t);
  strftime(buff, sizeof(buff), "%X", tmp);

  sem_wait(semLog);
  fprintf(f, "%s %s\n", buff, log);
  sem_post(semLog);
  printf("\n%s %s\n", buff, log);

}
