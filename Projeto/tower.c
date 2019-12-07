#include "header.h"
void * twtimer(){
	Arr_q * aux, *tmp, *ant;
	ant=NULL;
	char ** memslots;
	while(1){
		//variavel de condição com o tempo
		aux=arr_q;
		while(aux){
			sem_wait(semShM);		///removable
			memslots=mem->slots;
			sem_post(semShM);			//removable
			if(aux->arr->fuel<=0 || strcmp(memslots[aux->slot],BYEBYE)==0){
				sem_wait(semShM);		///removable
				strcpy(mem->slots[aux->slot],BYEBYE);

				if(aux->arr->fuel<=0){
					mem->flights_redirected++;
				}
				sem_post(semShM);			//removable
				tmp=arr_q;
				if (!ant){
					if(tmp->next!=NULL){
						arr_q=tmp->next;
						free(tmp);
					}
					else{
							arr_q=NULL;
					}
				}
				else{
					tmp=aux;
					aux=aux->next;
					ant->next=tmp->next;
					free(tmp);
				}
			}
			else{
				aux->arr->fuel--;
				aux->arr->eta--;
				ant=aux;
				aux = aux->next;
			}

		}
		usleep(data.ut*1000);				//removable after adding conditional variavel
	}
}
void printarr(Arr_q* arrq){
	Arr_q* copy;
	copy = arrq;
	while(copy){
		printf("%s\n",copy->arr->code);
		copy = copy->next;
	}
}
Arr_q* addArrival(Arr_q * node, Arr_q * head){
	Arr_q *tmp, *ant;
  if (head==NULL){
		printf("1\n");
		head=node;
	}
  else{
			if (((head->arr->eta + data.L > node->arr->eta) && (head->arr->fuel - head->arr->eta > node->arr->fuel - node->arr->eta)) || (node->arr->emer == 1 && head->arr->emer == 0) ) {
			//if ((head->arr->fuel - head->arr->eta +head->arr->init > node->arr->fuel - node->arr->eta+node->arr->init)||(node->arr->emer == 1 && head->arr->emer == 0)) {
					printf("2\n");
	  			node->next = head;
          head=node;
      }
			// else if (node->arr->emer == 1 && head->arr->emer == 1){
			// 	ant=head;
			// 	tmp=head->next;
			// 	while ((head!=0) || (head->arr->fuel - head->arr->eta + head->arr->init <= node->arr->fuel - node->arr->eta + node->arr->init)) {
			// 			ant=tmp;
			// 			tmp=tmp->next;
			// 	}
			// 	node->next=tmp;
			// 	ant->next=node;
			// }
      else {
					printf("3\n");
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
void printdep(Dep_q* depq){
	Dep_q* copy;
	copy = depq;
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
	while(1){
		printf("tou a oubire\n");
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
				*(mem->slots+(int)msgd.mtype)=BYEBYE;
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
				printf("Torre: slot = %d\n",msgs.slot);
				msgsnd(mqid, &msgs, sizeof(msgs), 0);
				Arr_q* arr = (Arr_q*)malloc(sizeof(Arr_q));
				arr->arr = (Arrival*)malloc(sizeof(Arrival));
				strcpy(arr->arr->code,msgd.arr.code);
				arr->arr->eta = msgd.arr.eta;
				arr->arr->fuel = msgd.arr.fuel;
				arr->next=NULL;
				arr_q=addArrival(arr,arr_q);
				printarr(arr_q);
			if (A>data.A){
				char * frej = malloc(sizeof(char)*128);
				sprintf(frej, "[Control Tower] Flight %s was rejected (maximum Arrivals was reached).",msgd.arr.code);
				writeLog(f,frej);
				sem_wait(semShM);
				*(mem->slots+(int)msgd.mtype)="byebye";
				mem->flights_rejected++;
				sem_post(semShM);
				A--;
			}
		}
	}
}
