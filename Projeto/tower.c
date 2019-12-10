#include "header.h"

void tower(){
	int A = 0, D= 0; 	//number of Arrival and Departure flights that were not rejected
	#ifdef DEBUG
		printf("Control Tower created.\n");
	#endif
	Msg_deparr msgd;
	Msg_slot msgs;
	//dep_q = (Dep_q*)malloc(sizeof(Dep_q));
	dep_q = NULL;
	//arr_q = (Arr_q*)malloc(sizeof(Arr_q));
	arr_q = NULL;
	pthread_t ttimer, fselector;
	pthread_create(&ttimer,NULL,(void *)twtimer,NULL);
	pthread_create(&fselector,NULL,(void *)flight_selector,NULL);
	char frej[128];
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
				//printdep(dep_q);
			if(D>data.D){
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Departures was reached).",msgd.dep.code);
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
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Arrivals was reached).",msgd.arr.code);
				writeLog(f,frej);
				sem_wait(semShM);
				mem->slots[msgs.slot]=BYEBYE;
				mem->flights_rejected++;
				sem_post(semShM);
				A--;
			}
			else if (msgd.arr.fuel < msgd.arr.eta){
				sprintf(frej, "[Control Tower] Flight %s was rejected (fuel would not be enough).",msgd.arr.code);
				writeLog(f,frej);
				sem_wait(semShM);
				mem->slots[msgs.slot]=BYEBYE;
				mem->flights_rejected++;
				sem_post(semShM);
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
		int i = 0;
		while(aux){						//enquanto existir elementos na queue dos arrivals
			i++;

			if(aux->arr->eta<=0 && i >5){
				if(aux->arr->fuel > (((i-1)/2)*(data.L+data.dl))){
					aux->arr->eta = ((i-1)/2)*data.L;
					sem_wait(semShM);
					mem->slots[aux->slot]=setHolding((int)aux->arr->eta);
					sem_post(semShM);
				}
				else{
					sem_wait(semShM);
					mem->slots[aux->slot]=BYEBYE;
					sem_post(semShM);
				}
			}
			if(aux->arr->fuel<=0 || strcmp(mem->slots[aux->slot],BYEBYE)==0){		//se for para remover
				printf("Mandar embora %s\n",aux->arr->code);
				sem_wait(semShM);		///removable
				mem->slots[aux->slot]=BYEBYE;

				if(aux->arr->fuel<=0){
					mem->flights_redirected++;
				}

				sem_post(&mem->flights[aux->slot]);				//para thread ir ler shared memory

				sem_post(semShM);			//removable

				if (!ant){
					if(aux->next!=NULL){				//Im the first
						aux=aux->next;
						tmp=arr_q;
						arr_q=tmp->next;
						free(tmp);
						i--;
					}
					else{												//Im the only one
						aux=NULL;
						free(arr_q);
						arr_q=NULL;
						i--;
					}
				}
				else{
					if(aux->next!=NULL){				//Im in the midle
						tmp=aux;
						aux=aux->next;
						ant->next=tmp->next;
						free(tmp);
						i--;
					}
					else{												//Im the last
						aux =NULL;
						free(ant->next);
						ant->next=NULL;
						i--;
					}
				}
				//printarr(arr_q);
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
			if (((head->arr->eta  > node->arr->eta + data.L) || ((head->arr->eta < node->arr->eta) &&(head->arr->eta + data.L > node->arr->eta) && (head->arr->fuel - head->arr->eta > node->arr->fuel - node->arr->eta)))) {
	  			node->next = head;
          head=node;
      }
      else {
          ant=head;
          tmp=head->next;
          while ((tmp!=NULL) && ((tmp->arr->eta  + data.L < node->arr->eta) || ((tmp->arr->eta  + data.L > node->arr->eta) && (tmp->arr->eta < node->arr->eta) &&  (tmp->arr->fuel - tmp->arr->eta < tmp->arr->fuel - tmp->arr->eta)))) {
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
	Dep_q* tempd;
	Arr_q* tempa;
	int value;
	while (1){
		sem_wait(semTSe);
		sem_wait(semRuW);				//wait for runway to be clear
		tempd = dep_q;
		tempa = arr_q;
		if (arr_q!=NULL){
			#ifdef DEBUG
				printf("arr_q not null\n" );
			#endif
			if ((dep_q==NULL) && arr_q->arr->eta <=0){
				#ifdef DEBUG
					printf("dep_q null, run arriv\n" );
				#endif
				//ver se possivel juntar par
				sem_wait(semArr);
				sem_getvalue(semDep, &value);
				if (value==2){
					sem_wait(semDep);
					sem_wait(semDep);
				}
				sem_wait(semShM);
				mem->slots[arr_q->slot]=DOURJOB;
				sem_post(&mem->flights[arr_q->slot]);				//para thread ir ler shared memory
				mem->flights_landed++;
				mem->time2land = (mem->time2land + abs(arr_q->arr->eta))/mem->flights_landed;
				sem_post(semShM);
				if (tempa->next!=NULL){
					arr_q = tempa->next;
					free(tempa);
				}
				else{
					arr_q=NULL;
				}
			}
			else if ((dep_q!=NULL) && ((arr_q->arr->eta <= dep_q->dep->takeoff - mem->t && arr_q->arr->emer == 0) || (arr_q->arr->emer == 1 && arr_q->arr->eta <= dep_q->dep->takeoff - mem->t + data.T))){
				//printf("arr_q and dep_q not null\n" );
				if ( arr_q->arr->eta <=0){
					#ifdef DEBUG
						printf("arr_q and dep_q not null, run arriv\n" );
					#endif
					//ver se possivel juntar par
					sem_wait(semArr);
					sem_getvalue(semDep, &value);
					if (value==2){
						sem_wait(semDep);
						sem_wait(semDep);
					}
					sem_wait(semShM);
					mem->slots[arr_q->slot]=DOURJOB;
					sem_post(&mem->flights[arr_q->slot]);				//para thread ir ler shared memory
					mem->flights_landed++;
					mem->time2land = (mem->time2land + abs(arr_q->arr->eta))/mem->flights_landed;
					sem_post(semShM);
					if (tempa->next!=NULL){
						arr_q = tempa->next;
						free(tempa);
					}
					else{
						arr_q=NULL;
					}
				}
			}
			else if ((dep_q!=NULL) && dep_q->dep->takeoff <= mem->t){
				#ifdef DEBUG
					printf("arr_q and dep_q not null, run dep\n" );
				#endif
				//ver se possivel juntar par
				sem_wait(semDep);
				sem_getvalue(semArr, &value);
				if (value==2){
					sem_wait(semArr);
					sem_wait(semArr);
				}
				sem_wait(semShM);
				mem->slots[dep_q->slot]=DOURJOB;
				sem_post(&mem->flights[dep_q->slot]);				//para thread ir ler shared memory
				mem->flights_takingoff++;
				mem->time2takeoff = (mem->time2takeoff + (int)(mem->t-dep_q->dep->takeoff))/mem->flights_takingoff;
				sem_post(semShM);
				if (tempd->next!=NULL){
					dep_q = tempd->next;
					free(tempd);
				}
				else{
					dep_q=NULL;
				}
			}
			else {//incrementar sepaforo
				sem_post(semRuW);
					//printf("nothing\n" );
			}
		}
		else if ((dep_q!=NULL) && (dep_q->dep->takeoff <= mem->t)){
			#ifdef DEBUG
				printf("arr_q null, run dep\n" );
			#endif

			sem_wait(semDep);
			sem_getvalue(semArr, &value);
			if (value==2){
				sem_wait(semArr);
				sem_wait(semArr);
			}
			sem_wait(semShM);
			mem->slots[dep_q->slot]=DOURJOB;
			sem_post(&mem->flights[dep_q->slot]);				//para thread ir ler shared memory
			mem->flights_takingoff++;
			mem->time2takeoff = (mem->time2takeoff + (int)(mem->t-dep_q->dep->takeoff))/mem->flights_takingoff;
			sem_post(semShM);
			if (tempd->next!=NULL){
				dep_q = tempd->next;
				free(tempd);
			}
			else{
				dep_q=NULL;
			}
		}
		else{//incrementar sepaforo
			sem_post(semRuW);
			//printf("nothing\n" );
		}
	}
	pthread_exit(NULL);
}
