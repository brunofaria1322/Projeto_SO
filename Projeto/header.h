#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/msg.h>

#define MAX 256
//#define DEBUG   //remove this line to remove debug messages
#define SEMLOG "SemLog"
#define SEMSHM "SemShM"
#define PIPE_NAME   "input_pipe"
#define NO_INST "no_inst"
#define BYEBYE "byebye"


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
	char code[8];
	float init;
	float takeoff;
}Departure;

typedef struct Dep_q{
	Departure* dep;
	struct Dep_q* next;
}Dep_q;

typedef struct{
	char code[8];
	float init;
	float eta;
	float fuel;
  int emer;
}Arrival;

typedef struct Arr_q{
	Arrival* arr;
  int slot;
	struct Arr_q* next;
}Arr_q;

typedef struct{
	long mtype;
	Departure dep;
	Arrival arr;
}Msg_deparr;

typedef struct{
  long mtype;
  int slot;
}Msg_slot;

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
  int flights_takingoff;    //Total number of flights that took off
  int time2takeoff;         //Average wait time to take off
  int hm;                   //Average number of holding maneuvers per landing flight
  int hm_emergency;         //Average number of holding maneuvers per emergency flight
  int flights_redirected;   //Number of flights redirected to another airport
  int flights_rejected;     //Flights rejected by the Control Tower
  char* slots[16];	        //Instructions for each flight

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
void tower();
void writeLog(FILE *f, char *log);
char* command(int argc, char argv[][MAX]);
commands * verify (int argc, char argv[][MAX], commands * head);

void *fDepart(Departure * departure);
void *fArrival(Arrival * arrival);
void sigint(int signum);
void showStats (int signum);
//void fixInput(char *string);

//Global Variables
Data data;            //Config
FILE *f;              //log file
int t;                //time
pid_t pid;            //distinct father from child process
int mqid;             //id of message queue
int queue_size;       //number of active threads
int semid;            //id of semaphore
int shmid;            //id of shared memory
SharedMemory *mem;    //pointer to the shared memory
//pthread_mutex_t shm_mutex;  //mutex for access to shared memoru
//pthread_mutex_t log_mutex;  //mutex for access to shared memoru
//pthread_mutex_t shm_mutex;  //mutex for access to shared memoru
sem_t *semLog, *semShM;
                    //, *semMQ, *semArrival, *semDepart;
Dep_q* dep_q;
Arr_q* arr_q;
