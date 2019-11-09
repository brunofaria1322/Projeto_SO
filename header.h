#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define MAX 256
#define DEBUG   //remove this line to remove debug messages
#define PIPE_NAME   "input_pipe"

typedef struct{
  int ut;   //Unity of Time (milisseconds)
  int T;    //Takeoff duration (in ut)
  int dt;   //Duration betwen Takeoffs (in ut)
  int L;    //Landing duration(in ut)
  int dl;   //Duration between Ladings (in ut)
  int min;  //holding - MINimum duration (in ut)
  int max;  //holding - MAXimum duration (in ut)
  int D;    //number max of Departs of the sistem
  int A;    //number max of Arrivals of the sistema
}Data;
typedef struct{
	char* code; 
	float init; 
	float takeoff;
}Departure;
typedef struct{
	char* code;
	float init;
	float eta;
	float fuel;
}Arrival;
void printData(Data data);
Data readConfig(Data data);
void torre();
void writeLog(char *log);
void pipew(char* argv[]);
char* command(int argc, char *argv[]);
//void fixInput(char *string);
