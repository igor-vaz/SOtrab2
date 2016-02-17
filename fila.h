#include <stdio.h>
#include <stdlib.h>

//estrutura de uma thread

typedef struct{
	int id; //id da thread
	int frame[4];
	int posMem;
  FILE *output;
}t_Args;

typedef struct {
    int capacidade;
    t_Args *buffer;
    int primeiro, ultimo;
    int nItens;
}Fila;

void criarFila(Fila *f, int c) {
    f->capacidade = c;
    f->buffer = (t_Args*)malloc(sizeof(t_Args) * f->capacidade);
    f->primeiro = 0;
    f->ultimo = -1;
}
void inserir(Fila *f, t_Args v) {
	if(f->ultimo == f-> capacidade - 1)
		f->ultimo = -1;
	f->ultimo++;
	f->buffer[f->ultimo] = v;
	f->nItens++;
}

t_Args remover(Fila *f) {
	t_Args temp = f->buffer[f->primeiro++];
	if(f->primeiro == f->capacidade)
		f->primeiro = 0;
	f->nItens--;
	return temp;

}
