#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)
//                    |
//    +----------+----+------------+
//    |          |                 |
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// pai (ao criar filho): "Contador: %d\n"
// pai (ao criar filho): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads
// - cada thread deve incrementar contador_global n_threads*1000
// - pai deve esperar pelas worker threads  antes de imprimir!

void *count(void *dummyPtr);
int contador_global = 0;
int counter=0;
int main(int argc, char* argv[]) {

//  int num = pthread_self();
  pthread_t thread_id[atoi(argv[1])];
  counter = atoi(argv[1]);
    if (argc < 2) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %d n_threads\n", atoi(argv[1]));
        return 1;
    }
    //printf("tamanho de argv %d\n",atoi(argv[1]));
    for (int i=0; i < *argv[1]; i++){
      printf("num da thread %d\n",i);
        pthread_create(&thread_id[i], NULL, count, NULL);
        //pthread_join(thread_id[i], NULL);
      //
      }

    for (int i=0; i < *argv[1]; i++){
      printf("num da thread %d\n",i);
        pthread_join(thread_id[i], NULL);
        //printf("Thread encerrada retorno \n");
      }

    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", counter);
    return 0;

}


void *count(void *dummyPtr){
  contador_global++;
  return 0;
}
