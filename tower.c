#include "header.h"
void tower(){
	queue_size = 0;
	#ifdef DEBUG
		printf("Control Tower created.\n");
	#endif

	Msg_deparr msgd;
	while(1){
		msgrcv(mqid, &msgd, sizeof(msgd)-sizeof(long), 0, 0);
		fflush(stdout);
		if (strcmp(msgd.dep.code,"\0")!=0){
			printf("[Control Tower] Flight %s will takeoff at %f\n",msgd.dep.code,msgd.dep.takeoff);
		}
		else if (strcmp(msgd.arr.code, "\0")!=0){
			printf("[Control Tower] Flight %s has an eta of %f and %f fuel\n",msgd.arr.code,msgd.arr.eta, msgd.arr.fuel);
		}
	}
	
}
