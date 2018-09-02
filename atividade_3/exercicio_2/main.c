
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
    double *c;
    int position;

};
void *sum(void *arg);
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
    int silent = argc > 4 && strcmp(argv[4], "-silent") == 0;
                           //^--->  0 se argv[4] == "-silent"
                           //|---> -1 se argv[4] <  "-silent"
                           //+---> +1 se argv[4] >  "-silent"

    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
    //Cria vetor do  resultado
    double* c = malloc(a_size*sizeof(double));

    // Calcula com uma thread só. Programador original só deixou a leitura
    // do argumento e fugiu pro caribe. É essa computação que você precisa
    // paralelizar
    pthread_t thread_id[n_threads];
    struct arg_struct pointers;

    pointers.a=a;
    pointers.b=b;
    pointers.c=c;


    //struct arg_struct *args = malloc(sizeof(struct arg_struct));
    for (int i = 0; i < n_threads; ++i) {
        int rec=(int)a_size;
        pointers.position = (i%rec);
        pthread_create(&thread_id[i], NULL, sum , (void*)&pointers);
        pthread_join(thread_id[i], NULL);
    }


    //Imprime resultados
    if (!silent) {
        for (int i = 0; i < a_size; ++i)
            printf("%s%g", i ? " " : "", c[i]);
        printf("\n");
    }

    //Importante: libera memória
    free(a);
    free(b);
    free(c);

    return 0;
}


void *sum(void *arg){
 int posit = (*((struct arg_struct*)arg)).position;
 double *a = (*((struct arg_struct*)arg)).a;
 double *b = (*((struct arg_struct*)arg)).b;
 double *c = (*((struct arg_struct*)arg)).c;

  pthread_mutex_lock(&mutex);
  c[posit] = a[posit] + b[posit];
  pthread_mutex_unlock(&mutex);
  return NULL;

}
