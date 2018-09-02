
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// |         usado como 2o retorno! <-----+
// v                                      v
double* load_vector(const char* filename, int* out_size);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct arg_struct {
    double *a;
    double *b;
    double c;
    int position;
    int jobPerThread;
    int *mod;
    int sizeOfA;

};
void *sum(void *arg);
void *sumOdd(void *arg);
int main(int argc, char* argv[]) {
    srand(time(NULL));

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file [-silent]\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n"
               "    -silent      não imprime resultado na saída\n", argv[0]);
        return 1;
    }

    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    //int silent = argc > 4 && strcmp(argv[4], "-silent") == 0;
                           //^--->  0 se argv[4] == "-silent"
                           //|---> -1 se argv[4] <  "-silent"
                           //+---> +1 se argv[4] >  "-silent"

    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
  struct arg_struct pointers;
    pointers.a=a;
    pointers.b=b;
    pointers.c=0;
    pointers.jobPerThread=0;
    pointers.mod=0;
    pointers.sizeOfA=0;
  if (n_threads > a_size){
      n_threads=a_size;
    } else if (n_threads <= a_size){
        int mod=a_size%n_threads;
        int quocient = a_size/n_threads;
        if(mod==0){
          pointers.jobPerThread=quocient;
          printf("MODEs %d\n",quocient);
          pthread_t thread_id[n_threads];

          for (int i = 0; i < n_threads; ++i) {
              pointers.position = i;
              printf("===================  %d\n",pointers.position);
              pthread_create(&thread_id[i], NULL, sum , (void*)&pointers);
              pthread_join(thread_id[i], NULL);
            }


        } else {

          pointers.jobPerThread=quocient;
          pointers.sizeOfA=a_size;
          printf("MODEs %d\n",quocient);
          pthread_t thread_id[n_threads];
          pointers.mod = &mod;
          for (int i = 0; i < n_threads; ++i) {
              pointers.position = i;
              printf("===================  %d\n",pointers.position);
              pthread_create(&thread_id[i], NULL, sumOdd , (void*)&pointers);
              pthread_join(thread_id[i], NULL);
            }
        }
      }
    //Imprime resultados
    double resultado = pointers.c;
        printf("Produto escalar: %g\n", resultado);

    //Importante: libera memória
    free(a);
    free(b);
    return 0;
   }
 void *sum(void *arg){
 int i = (*((struct arg_struct*)arg)).position;
 double *a = (*((struct arg_struct*)arg)).a;
 double *b = (*((struct arg_struct*)arg)).b;
 double c = (*((struct arg_struct*)arg)).c;
 int quocient = (*((struct arg_struct*)arg)).jobPerThread;
 for (int positions = i*quocient; positions < (i*quocient)+quocient; positions++){
      double pa=(a[positions]);
      double pb=(b[positions]);
      c += pa*pb;
  }

  (*((struct arg_struct*)arg)).c=c;

  return NULL;

}

void *sumOdd(void *arg){
 int i = (*((struct arg_struct*)arg)).position;
 double *a = (*((struct arg_struct*)arg)).a;
 double *b = (*((struct arg_struct*)arg)).b;
 double c = (*((struct arg_struct*)arg)).c;
 int quocient = (*((struct arg_struct*)arg)).jobPerThread;
 int *mods = (*((struct arg_struct*)arg)).mod;
 int sizeA = (*((struct arg_struct*)arg)).sizeOfA;
 for (int positions = i*quocient; positions < (i*quocient)+quocient; positions++){
      double pa=(a[positions]);
      double pb=(b[positions]);
      c += pa*pb;
  }
  if (*mods > 0){
    c += a[sizeA-*mods]*b[sizeA-*mods];
    (*mods)--;
  }
  (*((struct arg_struct*)arg)).c=c;
  return NULL;

}
