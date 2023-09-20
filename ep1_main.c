#include <stdio.h>
#include <stdlib.h>
#include <pthreads>
#include "spend_time.h"

/***********************************
 ************ Resources ************
 ***********************************/

#define RES_NUM 8
#define MAX_THREADS 1000

typedef struct thread_data_t {
  int tid;
  int f_time;
  int c_time;

  int res_v[RES_NUM];
  int res_c;
}

/***********************************
 ************ Behaviors ************
 ***********************************/

void init_recursos() {
  
}

void trava_recursos() {
  
}

void libera_recursos() {
  
}

void thread_func(void* args) {
  thread_data_t* data = (thread_data_t*) args;

  spend_time(data->tid, NULL, data->f_time);
  trava_recursos();
  spend_time(data->tid, 'C', data->f_time);
  libera_recursos();

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
        // create thread: TODO
        break;
      }
      res_c++;
      curr_int = next_int;
    }
  }
}
