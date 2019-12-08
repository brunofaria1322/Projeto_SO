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
#define DEBUG   //remove this line to remove debug messages
#define SEMLOG "SemLog"
#define SEMSHM "SemShM"
#define SEMARR "SemArr"
#define SEMDEP "SemDep"
#define SEMRUW "SemRuW"     //if runway is clear for or not
#define SEMTIM "SemTim"
#define PIPE_NAME   "input_pipe"
#define NO_INST "no_inst"
#define BYEBYE "redirected"
#define DOURJOB "completed"
#define MAKETIME "holding"


typedef struct{
  int ut;   //Unity of Time (milisseconds)
  int T;    //Takeoff duration (in ut)
  int dt;   //Duration betwen Takeoffs (in ut)
  int L;    //Landing duration(in ut)
  int dl;   //Duration between Ladings (in ut)
  int min;  //holding - MINimum duration (in ut)
  int max;  //holding - MAXimum duration (in ut)
  int D;    //number max of Departs of the sistem
  int A;    //number max of Arrivals of the sistem
}Data;


typedef struct{
	char code[8];
	float init;
	float takeoff;
}Departure;

typedef struct Dep_q{
	Departure* dep;
  int slot;
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
  int t;                    //timer
  sem_t flights[];          //unnamed semaphores for thread dep or arrival know when to acess shared memory
}SharedMemory;

typedef struct{
    int ut;
    commands * head;
}info;

//main
commands* addCommand(commands * node, commands * head);
commands* removeFirstCommand(commands * head);
char* command(int argc, char argv[][MAX]);
void ftimer(info * inf);
void sigint(int signum);
void showStats (int signum);
void *fDepart(Departure * departure);
void *fArrival(Arrival * arrival);
char * setHolding(int delay);
int getHolding(char* holding);
//float getTime(int ut);
void printData(Data data);
//readConfig
Data readConfig(Data data);
//tower
void tower();
void * twtimer();
Dep_q* addDeparture(Dep_q * node, Dep_q * head);
Arr_q* addArrival(Arr_q * node, Arr_q * head);
void printdep(Dep_q* copy);
void printarr(Arr_q* copy);
int insert_slot(char* slots[16], char* inst);
void* flight_selector();
//writeLog
void writeLog(FILE *f, char *log);
//verify
commands * verify (int argc, char argv[][MAX], commands * head);
//void fixInput(char *string);

//Global Variables
Data data;            //Config
FILE *f;              //log file
//int t;                //time
pid_t pid;            //distinct father from child process
int mqid;             //id of message queue
int queue_size;       //number of active threads
int semid;            //id of semaphore
int shmid;            //id of shared memory
SharedMemory *mem;    //pointer to the shared memory
//pthread_mutex_t shm_mutex;  //mutex for access to shared memoru
//pthread_mutex_t log_mutex;  //mutex for access to shared memoru
//pthread_mutex_t shm_mutex;  //mutex for access to shared memoru
sem_t *semLog, *semShM, *semArr, *semDep, *semTim, *semRuW;
                    //, *semMQ,
Dep_q* dep_q;
Arr_q* arr_q;
