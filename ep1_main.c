#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "spend_time.h"

/***********************************
 ************ Resources ************
 ***********************************/

#define RES_NUM 8
unsigned long resource_owner[RES_NUM];
pthread_mutex_t resource_mutex[RES_NUM];
pthread_cond_t resource_cond[RES_NUM]; 

pthread_mutex_t managing_res = PTHREAD_MUTEX_INITIALIZER;

#define MAX_THREADS 1000
pthread_t threads[MAX_THREADS];

typedef struct thread_data {
  int tid;
  int f_time;
  int c_time;

  int res_v[RES_NUM];
  int res_c;
} thread_data_t ;

/***********************************
 ************ Behaviors ************
 ***********************************/

void init_recursos() {
  for (int i=0; i<RES_NUM; i++) {
    resource_owner[i] = 0;
    pthread_mutex_init(&resource_mutex[i], NULL);
    pthread_cond_init(&resource_cond[i], NULL);
  }
}

void trava_recursos(int* res_v, int res_c) {
  pthread_mutex_lock(&managing_res);

  // acquire all resources
  int acquired_all = 0;
  while(!acquired_all) {
    acquired_all = 1;

    for (int i=0; i<res_c; i++) {
      int result = pthread_mutex_trylock(&resource_mutex[res_v[i]]);
      if (result != 0) {
        // if a resource is occupied, release all taken and wait
        for (int j=0; j<i; j++) {
          pthread_mutex_unlock(&resource_mutex[res_v[j]]);
        }
        acquired_all = 0;
        // printf("-------> waiting for res %d\n", res_v[i]);
        pthread_cond_wait(&resource_cond[res_v[i]], &managing_res);
        break;
      }
    }
  }

  // mark ownership
  unsigned long tid = pthread_self();
  // printf("saving tid %d\n", tid);
  for (int i=0; i<res_c; i++) {
    resource_owner[res_v[i]] = tid;
  }
  
  // printf("new allocation\n");
  // for (int i=0; i<RES_NUM; i++) {
  //   printf("\tres %d -> %d\n", i, resource_owner[i]);
  // }

  pthread_mutex_unlock(&managing_res);
}

void libera_recursos() {

  pthread_mutex_lock(&managing_res);
  
  unsigned long owner = pthread_self();
  // printf("freeing tid %d\n", owner);
  // first unlocks all used mutexes
  for (int i=0; i<RES_NUM; i++) {
    // printf("\t%d == %d ? %d\n", resource_owner[i], owner, resource_owner[i] == owner);
    if (resource_owner[i] == owner) {
      pthread_mutex_unlock(&resource_mutex[i]);
    }
  }
  // then signal waiting threads to proceed
  for (int i=0; i<RES_NUM; i++) {
    if (resource_owner[i] == owner) {
      // printf("-------> signalling %d is free\n", i);
      pthread_cond_signal(&resource_cond[i]);
    }
  }
  // there is no need to erase the owner
  pthread_mutex_unlock(&managing_res);
}

void* thread_func(void* args) {
  thread_data_t* data = (thread_data_t*) args;
  
  // printf("thread %d\n", data->tid);
  // printf("%d\n", data->f_time);
  // printf("%d\n", data->c_time);
  // printf("%d\n", data->res_c);
  
  spend_time(data->tid, NULL, data->f_time);

  // printf("%d -> acquiring\n", data->tid);
  
  trava_recursos(data->res_v, data->res_c);

  // printf("%d -> acquired\n", data->tid);
  
  spend_time(data->tid, "C", data->c_time);

  libera_recursos();

  // printf("%d -> freed\n", data->tid);
  
  free(data);
  pthread_exit(NULL);
}

void main() {
  
  init_recursos();
  
  char  line[128];
  char* curr_int;
  char* next_int;
  
  int tid;
  int f_time;
  int c_time;
  int res_v[RES_NUM];
  int res_c;
  //printf("starting\n");
  while (fgets(line, sizeof(line), stdin)) {
    curr_int = line;
    
    tid = strtol(curr_int, &next_int, 10);
    curr_int = next_int;
    
    f_time = strtol(curr_int, &next_int, 10);
    curr_int = next_int;
    
    c_time = strtol(curr_int, &next_int, 10);
    curr_int = next_int;

    if (tid == 0)
      break;
    res_c = 0;
    while (res_c <= RES_NUM) {
      res_v[res_c] = strtol(curr_int, &next_int, 10);
      if (curr_int == next_int || res_c == RES_NUM) {
        thread_data_t* args = (thread_data_t*) malloc(sizeof(thread_data_t));
        args->tid = tid;
        args->f_time = f_time;
        args->c_time = c_time;
        for (int i=0; i<res_c; i++) {
          args->res_v[i] = res_v[i];
        }
        args->res_c = res_c;

        pthread_create(&threads[tid-1], NULL, thread_func, args);
        // printf("thread made\n");
        break;
      }
      res_c++;
      curr_int = next_int;
    }
  }
  pthread_exit(NULL);
}
