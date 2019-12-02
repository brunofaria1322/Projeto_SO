#include "header.h"
void tower(){
	#ifdef DEBUG
		printf("Control Tower created.\n");
	#endif

	printf("mqid=%d\n",mqid);
	while(running){
		Msg_deparr msgd;
		printf("%d\n",msgrcv(mqid, &msgd, sizeof(msgd)-sizeof(long), 1, 0));
		printf("%lf\n",msgd.mtype);
			if (msgd.dep){
				printf("Control Tower: Flight %s will takeoff at %f",msgd.dep->code,msgd.dep->takeoff);
			}
	}

	exit(0);
}
