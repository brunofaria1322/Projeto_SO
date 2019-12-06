#include "header.h"
void printdep(Dep_q* depq){
	Dep_q* copy = (Dep_q*)malloc(sizeof(Dep_q));
	copy = depq;
	printf("%s\n",copy->dep->code);
	while(copy->next){
		copy = copy->next;
		printf("%s\n",copy->dep->code);
	}
	free(copy);
}
Dep_q* addDeparture(Dep_q * node, Dep_q * head){
	Dep_q *tmp, *ant;
  if (head==NULL){
	printf("1\n");
	head=node;
}
  else{
      if (head->dep->takeoff>node.dep->takeoff) {
					printf("2\n");
	  			node->next = head;
          head=node;
      }
      else {
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
		msgrcv(mqid, &msgd, sizeof(msgd)-sizeof(long), 1, 0);
		fflush(stdout);
		if (strcmp(msgd.dep.code,"\0")!=0){
				printf("[Control Tower] Flight %s with planned takeoff at %f\n",msgd.dep.code,msgd.dep.takeoff);
				D++;
				pthread_mutex_lock(&shm);
				mem->flights_created++;
				msgs.slot = insert_slot(mem->slots,NO_INST);
				pthread_mutex_unlock(&shm);
				msgs.mtype = 1;
				msgsnd(mqid, &msgs, sizeof(msgs), 0);
				Dep_q* dep = (Dep_q*)malloc(sizeof(Dep_q));
				strcpy(dep->dep->code,msgd.dep.code);
				dep->dep->takeoff = msgd.dep.takeoff;
				addDeparture(dep,dep_q);
				printdep(dep_q);
			if (D>maxD){
				char * frej = malloc(sizeof(char)*128);
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Departures was reached).",msgd.dep.code);
				writeLog(f,frej);
				pthread_mutex_lock(&shm);
				*(mem->slots+(int)msgd.mtype)=BYEBYE;
				mem->flights_rejected++;
				pthread_mutex_unlock(&shm);
				D--;
			}

		}
		else{
				printf("[Control Tower] Flight %s with a planned eta of %f. Fuel: %f\n",msgd.arr.code,msgd.arr.eta, msgd.arr.fuel);
				A++;

				pthread_mutex_lock(&shm);
				mem->flights_created++;
				int slot = insert_slot(mem->slots,NO_INST);
				pthread_mutex_unlock(&shm);
				msgsnd(mqid, &slot, sizeof(int), msgd.mtype);
			if (A>maxA){
				char * frej = malloc(sizeof(char)*128);
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Arrivals was reached).",msgd.arr.code);
				writeLog(f,frej);
				pthread_mutex_lock(&shm);
				*(mem->slots+(int)msgd.mtype)="byebye";
				mem->flights_rejected++;
				pthread_mutex_unlock(&shm);
				A--;
			}
		}
	}
}
