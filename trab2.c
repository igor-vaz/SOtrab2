#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h" 
//estrutura de uma thread
typedef struct{
	int id; //id da thread
	int cont; //contador para guardar o  LRU
}t_Args;

void *aviso(void * id){
	int tid = *(int*)id;
	printf("thread %d criada\n", tid);
}

int main(int argc, char const *argv[]){

	int nThreads,nPaginas,t;

	if (argc < 3){
		printf("rodar com %s <#threads> <#paginas>\n", argv[0]);
		exit(0);
	}

	nThreads = atoi(argv[1]), nPaginas = atoi(argv[2]);
	printf("%d\n", nThreads);
	pthread_t tid[nThreads];

	for(t=0; t<nThreads; t++) {
		int *id = malloc(sizeof(int));
		*id = t;
		//cria threads
		if (pthread_create(&tid[t], NULL, aviso, (void *)id)) {
		  printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}
	//espera todas as threads terminarem
	for (t=0; t<nThreads; t++) {
    	if (pthread_join(tid[t], NULL)) {
        	printf("--ERRO: pthread_join() \n"); exit(-1); 
    	} 
  	} 

  	pthread_exit(NULL);
	return 0;
		
}