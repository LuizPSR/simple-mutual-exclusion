#include <stdio.h>
#include <stdlib.h>
#include <pthreads>
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
pthread_t threads[MAX_THREAD];

typedef struct thread_data_t {
  int f_time;
  int c_time;

  int res_v[RES_NUM];
  int res_c;
}

/***********************************
 ************ Behaviors ************
 ***********************************/

void init_recursos() {
  for (int i=0; i<RES_NUM; i++) {
    resource_owner[i] = 0;
    resource_mutex[i] = PTHREAD_MUTEX_INITIALIZE;
    resource_cond[i]  = PTHREAD_COND_INITIALIZER;
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
        pthread_cond_wait(&resource_cond[i], &managing_res);
      }
    }
  }

  // mark ownership
  unsigned long tid = pthread_self();
  for (int i=0; i<res_c; i++) {
    resource_owner[res_v[i]] = tid;
  }

  pthread_mutex_unlock(&managing_res);
}

void libera_recursos() {

  pthread_mutex_lock(&managing_res);
  
  unsigned long owner = pthread_self();
  // first unlocks all used mutexes
  for (int i=0; i<RES_NUM; i++) {
    if (resource_owner[i] == owner) {
      pthread_mutex_unlock(&resource_mutex[i]);
    }
  }
  // then signal waiting threads to proceed
  for (int i=0; i<RES_NUM; i++) {
    if (resource_owner[i] == owner) {
      pthread_signal(&resource_cond[i]);
    }
  }
  // there is no need to erase the owner
  pthread_mutex_unlock(&managing_res);
}

void thread_func(void* args) {
  thread_data_t* data = (thread_data_t*) args;

  spend_time(data->tid, NULL, data->f_time);
  trava_recursos();
  spend_time(data->tid, 'C', data->f_time);
  libera_recursos();

  free(data);

  pthread_exit()
}

int main() {
  
  init_recursos();
  
  char  line[128];
  char* curr_int;
  char* next_int;
  
  int tid;
  int f_time;
  int c_time;
  int res_v[RES_NUM];
  int res_c;
  while (fgets(line, sizeof(line), stdin)) {
    curr_int = line;
    
    tid = strtol(curr_int, &next_int, 10);
    curr_int = next_int;
    
    f_time = strtol(curr_int, &next_int, 10);
    curr_int = next_int;
    
    c_time = strtol(curr_int, &next_int, 10);
    curr_int = next_int;

    res_c = 0;
    while (res_c < RES_NUM) {
      res_v[res_c] = strtol(p, &e, 10);
      if (curr_int == next_int) {
        thread_data_t* args = (thread_data_t) malloc(sizeof(thread_data_t));
        args->f_time = f_time;
        args->c_time = c_time;
        for (int i=0; i<res_c; i++) {
          args->res_v[i] = res_v[i];
        }
        args->res_c = res_c;

        pthread_create(&threads[tid], NULL, thread_func, args);

        break;
      }
      res_c++;
      curr_int = next_int;
    }
  }
}
