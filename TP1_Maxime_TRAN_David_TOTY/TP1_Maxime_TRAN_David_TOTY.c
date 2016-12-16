#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

//David Toty
//Maxime Tran

char* chaine;
int count;
int nbchar;
char recherche;

/*

  ARG 1 = Longueur de la chaine
  ARG 2 = Le char recherché
  ARG 3 = Nombre de threads 

*/


#define printf_r(...)				\
  do{						\
    pthread_mutex_lock(&output_lock);		\
    printf(__VA_ARGS__);			\
    pthread_mutex_unlock(&output_lock);		\
  }while(0)


static pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg){

  //timer dans le thread 
  clock_t start = clock();
  int pid;
  int tid;
  int j;
  int cpt=0;
      
  pid = getpid();
  tid = pthread_self();
	
  int borne = nbchar/count;
  int debut = borne * (int)arg;

  for(j=debut;j<debut+borne;j++)
    if(chaine[j]==recherche)
      cpt++;

  clock_t end = clock();
  double time_e = (end - start)/(double)CLOCKS_PER_SEC;
  printf("thread n=%d son temps est : %f son count est : %d\n",(int)arg,time_e,count);

  return (void*)cpt;

}

int main(int argc, char *argv[]){
  pthread_t *tids_tbl;
  int state;
  int i,err;
  int somme=0;
	
  //count = sysconf(_SC_NPROCESSORS_ONLN);
  //count = (count < 1) ? 1 : count;
 
  if(argc != 4)
    perror("error");

  nbchar = atoi(argv[1]);
  recherche = argv[2][0];
  chaine = malloc(sizeof (char)*nbchar);
  count = atoi (argv[3]);

  tids_tbl = malloc(sizeof(pthread_t) * count);
	
  if(tids_tbl == NULL){
    printf("Main: failed to allocate tids_tbl\n");
    exit(-1);
  }

  srand(getpid());
  for(i=0;i<nbchar;i++){
    chaine[i]= 'a'+ (random()%26);
  }

  chaine[nbchar-1]='\0';
	
  //printf("chaine : %s\nDebut du programme...\n",chaine); //affichage peut etre lourd
  // timer entre create et join
  //clock_t start = clock();
	
  for(i = 0; i < count; i++){
    err = pthread_create(&tids_tbl[i], NULL, thread_func, (void*) i);

    if(err){
      printf_r("Main: failed to create thread #%d, err %d\n", i, err);
      break;
    }
  }

  for(i = 0; i < count; i++){
    err = pthread_join(tids_tbl[i], (void**) &state);
		
    if(err)
      printf_r("Main: failed to join thread #%d, err %d\n", i, err);
    else{
      printf_r("Main: thread #%d has finished with return value %d\n", i, state);
      somme+=state;
    }
  }
	
  printf("Valeur total pour le char %c : %d\n",recherche,somme);
	
  // fin timer entre create et join
  //clock_t end = clock();
  //double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  //printf("temps = %f pour le thread n=%d\n", time_elapsed_in_seconds,count);


  //ecriture dans fichier, ne marche que pour environnement linux

  /*int fd = open ("resultats", O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
    char ecriture[50];
    sprintf(ecriture,"%d %d %ld\n",nbchar,count,temps);
	
    int k=0;
    while(ecriture[k]!='\0'){
    write (fd,&ecriture[k],1);
    k++;
    }
	

    close(fd);*/

  free(tids_tbl);
	
  return 0;
}
