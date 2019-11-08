#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAX 256

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

void printData(Data data);
Data readConfig(Data data);
void error(char *erro);
void torre();

//void fixInput(char *string);
