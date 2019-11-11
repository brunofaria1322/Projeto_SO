#include "header.h"

void writeLog(char *log){
  FILE *f;
  char buff[64];

  

  if ((f= fopen("log.txt" ,"a+")) == NULL){
      perror("Couldn't open the file log.txt");
      exit(1);
  }

  time_t t = time(0);
  struct tm *tmp = gmtime(&t);
  strftime(buff, sizeof(buff), "%X", tmp);

  fprintf(f, "%s %s\n", buff, log);
  printf("\n%s %s\n", buff, log);

  fclose(f);
}
