#include "header.h"
void tower(){
	signal(SIGINT,sigint);
	#ifdef DEBUG
		printf("Control Tower created.\n");
	#endif

	printf("mqid=%d\n",mqid);
	Msg_deparr msgd;
	while(1){
		printf("estou a espera de mensagens\n");
		msgrcv(mqid, &msgd, sizeof(Msg_deparr), 1, 0);
		printf("recebi uma mensagem");
		if (msgd.dep){
			printf("[Control Tower] Flight %s will takeoff at %f",msgd.dep->code,msgd.dep->takeoff);
		}
		else{printf("wtf?");}
	}

}
