#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <time.h> 
#include <semaphore.h>

#define N 64
//estrutura de uma thread

typedef struct{
	int id; //id da thread
	int frame[4];
	int posMem;
}t_Args;

int sequence[N];
int frames[N];
int cont;
sem_t mutex;

void printFrames(int frames[], int ws){
    int i = 0;
    for (i = 0; i < ws; i++){
        printf("%d ", frames[i]);    
    }
    printf("\n");
}

void reorganizaFrames(int frames[], int val, int pos, int ws){
    int i = 0;
    int posAloc = 0;
    int alocado = 0;
     
    // Pagina existe na memoria
    if (pos != ws){
        printf("Sem PF\n");
        printf("-----------\n");
        // Verifica se existe algum frame vazio
        for(i = 0; i < ws; i++){
    	    if(frames[i] == 0){
                posAloc = i-1;
                for(i = pos; i < ws-1; i++){
                    frames[i] = frames[i+1];
                } 
                frames[posAloc] = val;
                alocado = 1;
                break;
            }      
        }  
        // Nenhum frame vazio - Aloca no ultimo
        if(alocado == 0){
            for(i = pos; i < ws-1; i++){
                frames[i] = frames[i+1];
            }  
            frames[ws-1] = val;
        }
    }
    // Pagina nao existe na memoria e memoria cheia
    else{
        printf("PF\n");
        printf("-----------\n");
        for(i = 0; i < ws; i++){
    	    frames[i] = frames[i+1];
        }
        frames[ws-1] = val;
    }     
    printFrames(frames, ws);  
    alocado = 0;
    posAloc = 0;
}

void lru(int ws){
	/*int in = args->id % N;
    sequence[in] = args->id;*/
//    int sequence[N] = {1,2,3,4,5,6,4,5,1,2,3,4,5,6,9,10,2,3,4,5,6,7,2,11,1,2,4,7,9,10,11,21,10,2,1,1,4,3,4,5,1,2,3,4,5,6,9,10,2,3,4,5,6,7,2,11,1,2,4,7,9,10,11,21};
    //int frames[ws];
    int i = 0;
    int j = 0;

    // Inicia frames com 0
    for (i = 0; i < ws; i++){
        frames[i] = 0;
    }
     
    // Varrendo a lista
    //for (j = 0; j < sizeof(sequence)/sizeof(sequence[0]); j++){ 
        // Preenche os ws primeiros frames
        if(frames[ws-1] == 0){
                // Varre os frames da memoria
                for(i = 0; i <= ws; i++){
                    // Contem pagina - atualiza frames
                    if(frames[i] == sequence[j]){
                    	reorganizaFrames(frames, sequence[j], i, ws);   
                        break;  
                    }  
                    // Nao contem pagina - atualiza frames
                    else{
                        if(frames[i] == 0){
                            frames[i] = sequence[j];
                            printf("PF\n");
                            printf("-----------\n");
                            printFrames(frames, ws);
                            break;
                        }
                    }
                }
        }else{
            // varre os frames da memoria
            for(i = 0; i <= ws; i++){
                // Contem pagina - atualiza frames
                if(i < ws && frames[i] == sequence[j]){
                    reorganizaFrames(frames, sequence[j], i, ws);  
                    break; 
                } 
                // Nao contem pagina - atualiza frames
                else{
                    if(i == ws){
                        reorganizaFrames(frames, sequence[j], i, ws);
                        break;
                    }    
                } 
            } 
        }   
    //}
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
}

void *fazRequisicao(void *args){
	t_Args *arg = (t_Args*)args;
	int ws = 4;
	printf("thread %d criada\n", arg->id);
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
		printf("pagina %d criada\n", arg->frame[i]);
	}
	sem_wait(&mutex);
	//insere os frames da thread na memoria
	i = posicaoLivre();
	arg->posMem = i;
	for (k = i; k < i + ws; k++)
	{
		frames[k] = arg->frame[k-i];

	}
	printFrames(frames,N);

	sem_post(&mutex);

}


int main(int argc, char const *argv[]){
	
	// Working set Limit    
	int ws = 4;
	// Memory Limit
	int ml = 64;
	int nThreads,nPaginas,t;
	unsigned int secs;
  	t_Args *args;
  	cont = 0;
  	sem_init(&mutex, 0, 1);

	srand(time(NULL));
	
	if (argc < 3){
		printf("rodar com %s <#threads> <#paginas>\n", argv[0]);
		exit(0);
	}

	nThreads = atoi(argv[1]), nPaginas = atoi(argv[2]);
	
	pthread_t tid[nThreads];

	int i;

	for (i = 0; i < N; i++) frames[i]= -1;

	for(t=0; t<nThreads; t++){
		esperaPor(3);
		printf("criando thread no segundo %d \n", (t+1)*3);
    	args = (t_Args*)malloc(sizeof(t_Args));
		args->id = t;
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

  	pthread_exit(NULL);
	return 0;		
}