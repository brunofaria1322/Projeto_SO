#include "header.h"


Data readConfig(Data data){
  char l[MAX];
  int i,test;
  FILE *f;
  f= fopen("config.txt" ,"r");
  if (f == NULL){
      error("Couldn't open the file config.txt");
  }
  
  //TODO: Criar protetor para verificar o formato do config (1val, 2val, 2val, 2val, 1val 1val)
  
  for (i=0;i<6;i++){
    switch (i){

      //Apenas 1 valor
      case 0:   //ut
        fscanf(f,"%d",&data.ut);
        break;
      case 4:   //D
        fscanf(f,"%d",&data.D);
        break;
      case 5:   //A
        fscanf(f,"%d",&data.A);
        break;

      //Apenas 2 valores
      case 1:   //T, dt
        fscanf(f,"%d, %d",&data.T,&data.dt);
        break;
      case 2:   //L, dl
        fscanf(f,"%d, %d",&data.L,&data.dl);
        break;
      case 3:   //min, max
        fscanf(f,"%d, %d",&data.min,&data.max);
        break;
    }
  }
  /*
  if (!fgets (l, MAX, f)){
    error("Wrong format in config.txt");
  }
  */

  return data;
}
