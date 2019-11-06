#include "header.h"

int main(){
	Data data;
	data=readConfig(data);
	printData(data);
}


void error(char *erro){
	printf("Error: %s\n",erro);
	exit(-1);
}

void fixInput(char *string){
    string[strlen(string)-1]='\0';
}

void printData(Data data){
  printf("%d\n%d, %d\n%d, %d\n%d, %d\n%d\n%d\n",data.ut,data.T,data.dt,data.L,data.dl,data.min,data.max,data.D,data.A);
	printf("\n");
}
