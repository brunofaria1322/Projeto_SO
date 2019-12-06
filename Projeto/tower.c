#include "header.h"
void printdep(Dep_q* depq){
	Dep_q* copy;
	copy = depq;
	printf("oi\n" );
	while(copy){
		printf("%s\n",copy->dep->code);
		copy = copy->next;
	}
}
Dep_q* addDeparture(Dep_q * node, Dep_q * head){
	Dep_q *tmp, *ant;
  if (head==NULL){
		printf("1\n");
		head=node;
	}
  else{
      if (head->dep->takeoff>node->dep->takeoff) {
					printf("2\n");
	  			node->next = head;
          head=node;
      }
      else {
					printf("3\n");
          ant=head;
          tmp=head->next;
          while ((tmp!=NULL) && (tmp->dep->takeoff<node->dep->takeoff)) {
              ant=tmp;
              tmp=tmp->next;
          }
        	node->next=tmp;
          ant->next=node;
      }
		}
  return(head);
}

int insert_slot(char* slots[16], char* inst){
	int i;
	for(i=0;i<maxA+maxD;i++){
		if(slots[i]==NULL){
			slots[i]=inst;
			return i;
		}
	}
	return -1;
}
void tower(){
	int A = 0, D= 0; 	//number of Arrival and Departure flights that were not rejected
	#ifdef DEBUG
		printf("Control Tower created.\n");
	#endif
	Msg_deparr msgd;
	Msg_slot msgs;
	Dep_q* dep_q = (Dep_q*)malloc(sizeof(Dep_q));
	dep_q = NULL;
	//Arr_q* arr_q = (Arr_q*)malloc(sizeof(Arr_q));
	while(1){
		printf("tou a oubire\n");
		msgrcv(mqid, &msgd, sizeof(msgd), 0, 0);
		fflush(stdout);
		if (msgd.mtype == 1){
				printf("[Control Tower] Flight %s with planned takeoff at %f\n",msgd.dep.code,msgd.dep.takeoff);
				D++;
				pthread_mutex_lock(&shm_mutex);
				mem->flights_created++;
				msgs.slot = insert_slot(mem->slots,NO_INST);
				pthread_mutex_unlock(&shm_mutex);
				msgs.mtype = 3;
				printf("Torre: slot = %d\n",msgs.slot);
				msgsnd(mqid, &msgs, sizeof(msgs), 0);
				Dep_q* dep = (Dep_q*)malloc(sizeof(Dep_q));
				dep->dep = (Departure*)malloc(sizeof(Departure));
				strcpy(dep->dep->code,msgd.dep.code);
				dep->dep->takeoff = msgd.dep.takeoff;
				dep->next=NULL;
				dep_q=addDeparture(dep,dep_q);
				printdep(dep_q);
			if (D>maxD){
				char * frej = malloc(sizeof(char)*128);
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Departures was reached).",msgd.dep.code);
				writeLog(f,frej);
				pthread_mutex_lock(&shm_mutex);
				*(mem->slots+(int)msgd.mtype)=BYEBYE;
				mem->flights_rejected++;
				pthread_mutex_unlock(&shm_mutex);
				D--;
			}

		}
		else if (msgd.mtype == 2){
				printf("[Control Tower] Flight %s with a planned eta of %f. Fuel: %f\n",msgd.arr.code,msgd.arr.eta, msgd.arr.fuel);
				A++;

				pthread_mutex_lock(&shm_mutex);
				mem->flights_created++;
				msgs.slot = insert_slot(mem->slots,NO_INST);
				pthread_mutex_unlock(&shm_mutex);
				msgs.mtype = 3;
				msgsnd(mqid, &msgs, sizeof(msgs), 0);
			if (A>maxA){
				char * frej = malloc(sizeof(char)*128);
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Arrivals was reached).",msgd.arr.code);
				writeLog(f,frej);
				pthread_mutex_lock(&shm_mutex);
				*(mem->slots+(int)msgd.mtype)="byebye";
				mem->flights_rejected++;
				pthread_mutex_unlock(&shm_mutex);
				A--;
			}
		}
	}
}
