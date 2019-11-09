#include "header.h"


Data readConfig(Data data){
  //char l[MAX];
  int i;
  FILE *f;

  #ifdef DEBUG
		printf("Openning config.txt and reading is content\n");
	#endif

  if ((f= fopen("config.txt" ,"r")) == NULL){
      perror("Couldn't open the file config.txt");
      exit(1);
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
    perror("Wrong format in config.txt");
    exit(1);
  }
  */

	fclose(f);
  return data;
}
