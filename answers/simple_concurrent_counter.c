#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h> 
// thread
void Pthread_mutex_init(pthread_mutex_t *m, pthread_mutexattr_t * attr) 
{
    int rc = pthread_mutex_init(m, attr);
    assert(rc == 0);
}

void Pthread_mutex_lock(pthread_mutex_t *m) 
{
    int rc = pthread_mutex_lock(m);
    assert(rc == 0);
}

void Pthread_mutex_unlock(pthread_mutex_t *m) 
{
    int rc = pthread_mutex_unlock(m);
    assert(rc == 0);
}

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
    int rc = pthread_create(thread, attr, start_routine, arg);
    assert(rc == 0);
}

void Pthread_join(pthread_t thread, void **value_ptr)
{
    int rc = pthread_join(thread, value_ptr);
    assert(rc == 0);
}
// counter
typedef struct __counter_t {
    int val;
    pthread_mutex_t lock;
} counter_t;

void counter_init(counter_t *c) {
    c->val = 0;
    Pthread_mutex_init(&c->lock, NULL);
}

void counter_inc(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    c->val++;
    Pthread_mutex_unlock(&c->lock);
}

void counter_dec(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    c->val--;
    Pthread_mutex_unlock(&c->lock);
}

int counter_get(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    int val = c->val;
    Pthread_mutex_unlock(&c->lock);
    return val;
}

int counter_inc_and_get(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    c->val++;
    int val = c->val;
    Pthread_mutex_unlock(&c->lock);
    return val;
}

void counter_reset(counter_t *c) {
    Pthread_mutex_lock(&c->lock);
    c->val = 0;
    Pthread_mutex_unlock(&c->lock);
}


typedef struct __args_t {
    int loops;
    counter_t *counter;
} args_t;

void *CountTo1M(void *arg) {
    args_t *a = (args_t *) arg;
    int i;
    for (i = 0; i < a->loops; i++) {
        counter_inc(a->counter);
    }
    return NULL;
}

int main (int argc, char *argv[]) {
  int NUM_THREADS = strtol(argv[1], NULL, 10);
  pthread_t threads[NUM_THREADS];
  // long times[NUM_THREADS]={0};
  double total;
  int rc, t;
  counter_t counter;
  counter_init(&counter);
  args_t args;
  args.loops = 1000000;
  args.counter = &counter;
  struct timeval start, end;
  
  rc = gettimeofday(&start, NULL);
  if (rc) {
    printf("error from gettimeofday on start\n");
  }
  for (t = 0; t<NUM_THREADS;t++) {
    Pthread_create(&threads[t], NULL, CountTo1M, &args);
  }
  for (t=0; t<NUM_THREADS; t++) {
    Pthread_join(threads[t], NULL);
  }
  rc = gettimeofday(&end, NULL);
  if (rc) {
    printf("error from gettimeofday on end\n");
  }
  total = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
  printf("%dthreads with 100000 loops took %f seconds\n\n", NUM_THREADS, total / 1000000);
  printf("Counter Expected: \t%d\nActual: counter_get: \t%d\n", 1000000 * NUM_THREADS, counter_get(&counter));

  return 0;
}
