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
Dep_q* addDeparture(Departure * node, Dep_q * head){
	Dep_q *tmp, *ant;
  if (head->dep==NULL){
	printf("1\n");
	head->dep=node;
}
  else{
      if (head->dep->takeoff>node->takeoff) {
	printf("2\n");
	  head->next = head;
          head->dep=node;
      }
      else {
          ant=head;
          tmp=head->next;
          while ((tmp!=NULL) && (tmp->dep->takeoff<node->takeoff)) {
              ant=tmp;
              tmp=tmp->next;
          }
          ant->next->dep=node;
      }
		}
  return(head);
}

int insert_slot(char* slots[16], char* inst){
	int i;
	for(i=1;i<maxA+maxD+1;i++){
		if(*(slots+i)==NULL){
			*(slots+i)=inst;
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
	Dep_q* dep_q = (Dep_q*)malloc(sizeof(Dep_q));
	dep_q->dep = NULL; dep_q->next = NULL;
	//Arr_q* arr_q = (Arr_q*)malloc(sizeof(Arr_q));
	Departure* dep = (Departure*)malloc(sizeof(Departure));
	while(1){
		msgrcv(mqid, &msgd, sizeof(msgd)-sizeof(long), 0, 0);
		fflush(stdout);
		if (msgd.arr ==NULL){
				printf("[Control Tower] Flight %s with planned takeoff at %f\n",msgd.dep.code,msgd.dep.takeoff);
				D++;
				pthread_mutex_lock(&shm);
				mem->flights_created++;
				int slot = insert_slot(mem->slots,NO_INST);
				pthread_mutex_unlock(&shm);
				msgsnd(mqid, &slot, sizeof(int), msgd.mtype);
				strcpy(dep->code,msgd.dep.code);
				dep->takeoff = msgd.dep.takeoff;
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
