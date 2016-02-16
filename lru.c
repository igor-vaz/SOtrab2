#include <stdio.h>
#include <stdlib.h>

/*

  Simulacao de Gerenciador de Memoria - Algoritmo LRU 
  
  Trabalho 2 - Sistemas Operacionais
  Grupo: Alexandre Miloski, Igor Vaz, Victor Peres, Matheus Martins
  Data: 18/02/16
  
*/

// Working set Limit    
int ws = 4;

// Memory Limit
int ml = 64;

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


// Algoritmo LRU
// Entrada: Uma sequencia de paginas 
// Execucao: Gerência de paginas por LRU
// Saida: Cada interacao do LRU

void lru(int ws){
     int sequence[64] = {1,2,3,4,5,6,4,5,1,2,3,4,5,6,9,10,2,3,4,5,6,7,2,11,1,2,4,7,9,10,11,21,10,2,1,1,4,3,4,5,1,2,3,4,5,6,9,10,2,3,4,5,6,7,2,11,1,2,4,7,9,10,11,21};
     int frames[ws];
     int i = 0;
     int j = 0;
     
     // Inicia frames com 0
     for (i = 0; i < ws; i++){
         frames[i] = 0;
     }
     
     // Varrendo a lista
     for (j = 0; j < ws/*sizeof(sequence)/sizeof(sequence[0])*/; j++){
         
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
         }
         
         else{
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
     }
}

int main(int argc, char *argv[]){
    
  lru(ml);             
  
  //system("PAUSE");	
  return 0;
}
