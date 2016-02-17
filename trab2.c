#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include "fila.h"
#define N 64
#define MAX 50

// //estrutura de uma thread
//
// typedef struct{
// 	int id; //id da thread
// 	int frame[4];
// 	int posMem;
// }t_Args;

int sequence[N];
int frames[N];
int threadAtiva[MAX];
Fila filaDeThreads;
int cont;
sem_t mutex, mutexPrint;

void printFrames(int frames[], int ws, FILE *output){
    int i = 0;
    for (i = 0; i < ws; i++){
        printf("%d ", frames[i]);
        fprintf(output,"%d ", frames[i]);
    }
    printf("\n");
    fprintf(output,"\n");
}

void reorganizaFrames(int frame[], int val, int pos, int ws, FILE *output){
    int i = 0;
    int posAloc = 0;
    int alocado = 0;

    // Pagina existe na memoria
    if (pos != ws){
        sem_wait(&mutexPrint);
        printf("Sem PF\n");
        printf("-----------\n");
        fprintf(output,"Sem PF\n");
        fprintf(output,"-----------\n");
		sem_post(&mutexPrint);
        // Verifica se existe algum frame vazio
        for(i = 0; i < ws; i++){
    	    if(frame[i] == 0){
                posAloc = i-1;
                for(i = pos; i < ws-1; i++){
                    frame[i] = frame[i+1];
                }
                frame[posAloc] = val;
                alocado = 1;
                break;
            }
        }
        // Nenhum frame vazio - Aloca no ultimo
        if(alocado == 0){
            for(i = pos; i < ws-1; i++){
                frame[i] = frame[i+1];
            }
            frame[ws-1] = val;
        }
    }
    // Pagina nao existe na memoria e memoria cheia
    else{
    	sem_wait(&mutexPrint);
        printf("PF\n");
        printf("-----------\n");
        fprintf(output,"PF\n");
        fprintf(output,"-----------\n");
		sem_post(&mutexPrint);
        for(i = 0; i < ws; i++){
    	    frame[i] = frame[i+1];
        }
        frame[ws-1] = val;
    }
    sem_wait(&mutexPrint);
    printFrames(frame, ws, output);
	sem_post(&mutexPrint);
    alocado = 0;
    posAloc = 0;
}

void lru(int* frame, int valor, int ws, FILE *output) {
    int i = 0;
    int j = 0;

	// varre os frames da memoria
	for(i = 0; i <= ws; i++){
	    // Contem pagina - atualiza frames
	    if(i < ws && frame[i] == valor){
	        reorganizaFrames(frame, valor, i, ws, output);
	        break;
	    }
	    // Nao contem pagina - atualiza frames
	    else{
	        if(i == ws){
	            reorganizaFrames(frame, valor, i, ws, output);
	            break;
	        }
    	}
	}
}

void esperaPor (unsigned int segs) {
    unsigned int tempoRet = time(0) + segs;
    while (time(0) < tempoRet);
}

int criaPagina(){
	int n = rand() % 50;
	return n;
}

int posicaoLivre(){
	int i;
	for (i = 0; i < N; i++)
	{
		if (frames[i]==-1)
		{
			return i;
		}
	}
	return -1;
}

void *fazRequisicao(void *args){
	t_Args *arg = (t_Args*)args;
	int ws = 4;
	sem_wait(&mutexPrint);
	printf("thread %d criada\n", arg->id);
	fprintf(arg->output,"thread %d criada\n", arg->id);
	sem_post(&mutexPrint);
	//gera o numero de pagina
	int i,j,k;
	for ( i = 0; i < ws; i++)
	{
		int n = criaPagina();
		for (j = 0; j < i; j++) {
			while (n == arg->frame[j])
				n = criaPagina();
		}
		arg->frame[i] = n;
		sem_wait(&mutexPrint);
		printf("pagina %d criada na thread %d\n", arg->frame[i], arg->id);
		fprintf(arg->output,"pagina %d criada na thread %d\n", arg->frame[i], arg->id);
		sem_post(&mutexPrint);
	}
	sem_wait(&mutex);
	//insere os frames da thread na memoria
	i = posicaoLivre();
	if (i == -1) {
		t_Args proc;
	    proc = remover(&filaDeThreads);
	    sem_wait(&mutexPrint);
		printf("SWAP-OUT: processo %d\n", proc.id);
		fprintf(arg->output,"SWAP-OUT: processo %d\n", proc.id);
		sem_post(&mutexPrint);
	    arg->posMem = proc.posMem;
	    threadAtiva[proc.id] = 0;
		i = arg->posMem;
	}
	else {
	    arg->posMem = i;
	}
	for (k = i; k < i + ws; k++)
	{
	    frames[k] = arg->frame[k-i];
	}
	sem_wait(&mutexPrint);
	printf("thread arg: %p\n", arg);
	fprintf(arg->output,"thread arg: %p\n", arg);
	sem_post(&mutexPrint);
	inserir(&filaDeThreads, *arg);
	sem_wait(&mutexPrint);
	printFrames(frames,N, arg->output);
	sem_post(&mutexPrint);

	sem_post(&mutex);

	while (threadAtiva[arg->id]) {
		esperaPor(3);
		int n = criaPagina();
		sem_wait(&mutexPrint);
		printf("pagina %d solicitada pela thread %d\n", n, arg->id);
		fprintf(arg->output,"pagina %d solicitada pela thread %d\n", n, arg->id);
		sem_post(&mutexPrint);
		sem_wait(&mutex);

		lru(arg->frame, n, ws, arg->output);
		i = arg->posMem;
		for (k = i; k < i + ws; k++)
		{
			frames[k] = arg->frame[k-i];
		}
		sem_wait(&mutexPrint);
		printFrames(frames,N, arg->output);
		sem_post(&mutexPrint);

		sem_post(&mutex);
	}
	sem_wait(&mutexPrint);
	printf("O processo %d terminou\n", arg->id);
	fprintf(arg->output,"O processo %d terminou\n", arg->id);
	sem_post(&mutexPrint);
}


int main(int argc, char const *argv[]){
	FILE *output = fopen("output-thread.txt","w+");
	// Working set Limit
	int ws = 4;
	// Memory Limit
	int ml = 64;
	int nThreads,nPaginas,t;
	unsigned int secs;
  	t_Args *args;
  	cont = 0;
	criarFila(&filaDeThreads, MAX);
  	sem_init(&mutex, 0, 1);
  	sem_init(&mutexPrint, 0, 1);

	srand(time(NULL));

	if (argc < 3){
		printf("rodar com %s <#threads> <#paginas>\n", argv[0]);
		exit(0);
	}

	nThreads = atoi(argv[1]), nPaginas = atoi(argv[2]);

	pthread_t tid[nThreads];

	int i;

	for (i = 0; i < N; i++) frames[i]= -1;

	for (i = 0; i < MAX; i++) threadAtiva[i]= 1;

	for(t=0; t<nThreads; t++){
		esperaPor(3);
    	args = (t_Args*)malloc(sizeof(t_Args));
		args->id = t;
		args->output = output;
		sem_wait(&mutexPrint);
		printf("criando thread no segundo %d \n", (t+1)*3);
		fprintf(args->output,"criando thread no segundo %d \n", (t+1)*3);
		sem_post(&mutexPrint);
		//cria threads
		if (pthread_create(&tid[t], NULL, fazRequisicao, (void *)args)){
		  printf("--ERRO: pthread_create()\n"); exit(-1);
		}
	}
	//espera todas as threads terminarem
	for (t=0; t<nThreads; t++){
    	if (pthread_join(tid[t], NULL)){
        	printf("--ERRO: pthread_join() \n"); exit(-1);
    	}
  	}
  	fclose(output);
  	pthread_exit(NULL);
	return 0;
}
