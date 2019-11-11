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
//#define DEBUG   //remove this line to remove debug messages
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


typedef struct commands{
  Departure * dep;
  Arrival * arr;
  int init;
  struct commands * next;
}commands;

typedef struct{
  int flights_created;      //Total number of flights created
  int flights_landed;       //Total number of flights that landed
  int time2land;            //Average wait time (beyond ETA) to land
  int flights_takingoff;    //Total number of flights taking off
  int time2takeoff;         //Average wait time to take off
  int hm;                   //Average number of holding maneuvers per landing flight
  int hm_emergency;         //Average number of holding maneuvers per emergency flight
  int flights_redirected;   //Number of flights redirected to another airport
  int flights_rejected;     //Flights rejected by the Control Tower

  //extras
  char *id;                 //id of flight
  int fuel;                 //fuel of flight
  int eta;                  //estimated time of arrival
  int etd;                  //estimated time to depart

}SharedMemory;

typedef struct{
    int ut;
    commands * head;
}info;

commands* removeFirstCommand(commands * head);
commands* addCommand(commands * node, commands * head);
void ftimer(info * inf);
//float getTime(int ut);
void printData(Data data);
Data readConfig(Data data);
void torre();
void writeLog(char *log);
commands * verify(char state,char argv[][MAX], commands * head);  //state can be "d" for depart or "a" for arrival
char* command(int argc, char argv[][MAX]);
commands * verifica (int argc, char argv[][MAX], commands * head);

void *fDepart(Departure * departure);
void *fArrival(Arrival * arrival);
//void fixInput(char *string);
