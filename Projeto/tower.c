#include "header.h"

void tower(){
	int A = 0, D= 0; 	//number of Arrival and Departure flights that were not rejected
	#ifdef DEBUG
		printf("Control Tower created.\n");
	#endif
	Msg_deparr msgd;
	Msg_slot msgs;
	dep_q = (Dep_q*)malloc(sizeof(Dep_q));
	dep_q = NULL;
	arr_q = (Arr_q*)malloc(sizeof(Arr_q));
	arr_q = NULL;
	pthread_t ttimer;
	pthread_create(&ttimer,NULL,(void *)twtimer,NULL);
	while(1){
		//printf("tou a oubire\n");
		msgrcv(mqid, &msgd, sizeof(msgd), 0, 0);
		fflush(stdout);
		if (msgd.mtype == 1){
				printf("[Control Tower] Flight %s with planned takeoff at %f\n",msgd.dep.code,msgd.dep.takeoff);
				D++;
				sem_wait(semShM);
				mem->flights_created++;
				msgs.slot = insert_slot(mem->slots,NO_INST);
				sem_post(semShM);
				msgs.mtype = 3;
				msgsnd(mqid, &msgs, sizeof(msgs), 0);
				Dep_q* dep = (Dep_q*)malloc(sizeof(Dep_q));
				dep->dep = (Departure*)malloc(sizeof(Departure));
				strcpy(dep->dep->code,msgd.dep.code);
				dep->dep->takeoff = msgd.dep.takeoff;
				dep->slot = msgs.slot;
				dep->next=NULL;
				dep_q=addDeparture(dep,dep_q);
				printdep(dep_q);
			if (D>data.D || msgd.arr.fuel < msgd.arr.eta){
				char * frej = malloc(sizeof(char)*128);
				if(D>data.D){
					sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Departures was reached).",msgd.dep.code);
				}
				else{sprintf(frej, "[Control Tower] Flight %s was rejected (fuel would not be enough).",msgd.dep.code);}
				writeLog(f,frej);
				sem_wait(semShM);
				mem->slots[msgs.slot]=BYEBYE;
				mem->flights_rejected++;
				sem_post(semShM);
				D--;
			}

		}
		else if (msgd.mtype == 2){
				printf("[Control Tower] Flight %s with a planned eta of %f. Fuel: %f\n",msgd.arr.code,msgd.arr.eta, msgd.arr.fuel);
				A++;

				sem_wait(semShM);
				mem->flights_created++;
				msgs.slot = insert_slot(mem->slots,NO_INST);
				sem_post(semShM);
				msgs.mtype = 3;
				//printf("Torre: slot = %d\n",msgs.slot);
				msgsnd(mqid, &msgs, sizeof(msgs), 0);
				Arr_q* arr = (Arr_q*)malloc(sizeof(Arr_q));
				arr->arr = (Arrival*)malloc(sizeof(Arrival));
				strcpy(arr->arr->code,msgd.arr.code);
				arr->arr->eta = msgd.arr.eta;
				arr->arr->fuel = msgd.arr.fuel;
				arr->slot = msgs.slot;
				arr->next=NULL;
				arr_q=addArrival(arr,arr_q);
				printarr(arr_q);
			if (A>data.A){
				char * frej = malloc(sizeof(char)*128);
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Arrivals was reached).",msgd.arr.code);
				writeLog(f,frej);
				sem_wait(semShM);
				mem->slots[msgs.slot]=BYEBYE;
				mem->flights_rejected++;
				sem_post(semShM);
				A--;
			}
		}
	}
}

void * twtimer(){
	Arr_q * aux, *tmp, *ant;
	while(1){
		sem_wait(semTim); 		//espera por alteração no tempo
		ant=NULL;
		aux=arr_q;
		while(aux){						//enquanto existir elementos na queue dos arrivals

			if(aux->arr->fuel<=0 || strcmp(mem->slots[aux->slot],BYEBYE)==0){		//se for para remover
				//printf("Fuel tá a zero em %s\n",aux->arr->code);
				sem_wait(semShM);		///removable
				mem->slots[aux->slot]=BYEBYE;

				if(aux->arr->fuel<=0){
					mem->flights_redirected++;
				}

				sem_post(&mem->flights[aux->slot]);				//para thread ir ler shared memory

				sem_post(semShM);			//removable

				if (!ant){
					if(aux->next!=NULL){				//Im the first
						arr_q=tmp->next;
						free(tmp);
					}
					else{												//Im the only one
						aux=NULL;
						free(arr_q);
						arr_q=NULL;
					}
				}
				else{
					if(aux->next!=NULL){				//Im in the midle
						tmp=aux;
						aux=aux->next;
						ant->next=tmp->next;
						free(tmp);
					}
					else{												//Im the last
						aux =NULL;
						free(ant->next);
						ant->next=NULL;
					}
				}
				printarr(arr_q);
			}
			else{
				aux->arr->fuel--;
				aux->arr->eta--;
				ant=aux;
				aux = aux->next;
			}

		}
	}
}
void printarr(Arr_q* copy){
	while(copy){
		printf("%s\n",copy->arr->code);
		copy = copy->next;
	}
}
Arr_q* addArrival(Arr_q * node, Arr_q * head){
	Arr_q *tmp, *ant;
  if (head==NULL){
		//printf("1\n");
		head=node;
	}
  else{
			if (((head->arr->eta + data.L > node->arr->eta) && (head->arr->fuel - head->arr->eta > node->arr->fuel - node->arr->eta)) || (node->arr->emer == 1 && head->arr->emer == 0) ) {
	  			node->next = head;
          head=node;
      }
      else {
          ant=head;
          tmp=head->next;
          while ((tmp!=NULL) && ((head->arr->eta + data.L <= node->arr->eta) || (head->arr->fuel - head->arr->eta <= node->arr->fuel - node->arr->eta)) && (node->arr->emer == 0 || head->arr->emer == 1)) {
              ant=tmp;
              tmp=tmp->next;
          }
        	node->next=tmp;
          ant->next=node;
      }
		}
  return(head);
}
void printdep(Dep_q* copy){
	while(copy){
		printf("%s\n",copy->dep->code);
		copy = copy->next;
	}
}
Dep_q* addDeparture(Dep_q * node, Dep_q * head){
	Dep_q *tmp, *ant;
  if (head==NULL){
		head=node;
	}
  else{
      if (head->dep->takeoff>node->dep->takeoff) {
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
	for(i=0;i<data.A+data.D;i++){
		if(slots[i]==NULL){
			slots[i]=inst;
			return i;
		}
	}
	return -1;
}
void* flight_selector(){
	Dep_q* tempd = dep_q;
	Arr_q* tempa = arr_q;
	if ((arr_q->arr->eta <= dep_q->dep->takeoff - t && arr_q->arr->emer == 0) || (arr_q->arr->emer == 1 && arr_q->arr->eta <= dep_q->dep->takeoff - t + data.T)){
		sem_wait(semArr);
		sem_getvalue(semDep,0);
		mem->slots[arr_q->slot]=DOURJOB;
		if (tempa->next!=NULL){
			arr_q = tempa->next;
			free(tempa);
		}
		else{
			arr_q=NULL;
		}
	}
	else{
		sem_wait(semDep);
		sem_getvalue(semArr,0);
		mem->slots[dep_q->slot]=DOURJOB;
		if (tempd->next!=NULL){
			dep_q = tempd->next;
			free(tempd);
		}
		else{
			dep_q=NULL;
		}
	}
	return 0;
}
