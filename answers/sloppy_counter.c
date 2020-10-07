#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#define NUMCPUS 4 // count of usable process
// thread function with assert
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

// sloppy counter
typedef struct __scounter_t {
    int64_t global;                 // the global total
    pthread_mutex_t glock;          // the global lock
    int local[NUMCPUS];             // the cpu local total
    pthread_mutex_t llock[NUMCPUS]; // the cpu local lock
    int threshold;                  // the max value stored in local, before transfering to global. If threshold is high, lower frequency of transfering occurs. 
} scounter_t;

/* init: set threshold, init global/local totals to 0, init locks */
void init(scounter_t *c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    Pthread_mutex_init(&c->glock, NULL);
    int i;
    for (i = 0; i < NUMCPUS; i++) {
        c->local[i] = 0;
        Pthread_mutex_init(&c->llock[i], NULL);
    }
}

/* update: change the scounter_t value by amt. If the new 
 *      local total >= threshold, transfer to global 
 */
void update(scounter_t *c, int threadID, int amt) {
    int cpu = threadID % NUMCPUS;
    Pthread_mutex_lock(&c->llock[cpu]);     // local lock first
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold) {    // if local count is bigger than threshold, send it to global
        Pthread_mutex_lock(&c->glock);      // global lock second
        c->global += c->local[cpu];
        Pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    Pthread_mutex_unlock(&c->llock[cpu]);
}

/* get: return the global total.*/
int get(scounter_t *c) {
    Pthread_mutex_lock(&c->glock);
    int val = c->global;
    Pthread_mutex_unlock(&c->glock);
    return val;
}

typedef struct __args_t {
    int loops;
    scounter_t *counter;
    int mytid;
} args_t;

void *ThreadCounter(void *arg) {
    args_t *a = (args_t *) arg;
    int i;
    for (i = 0; i < a->loops; i++) {
        update(a->counter, a->mytid, 1);
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    int loops, num_threads, threshold;
    loops = 1000000;
    num_threads = strtol(argv[1], NULL, 10);
    threshold = strtol(argv[2], NULL, 10);

    printf("Number of threads:\t%d\nSize of threshold:\t%d\n",num_threads, threshold);

    pthread_t threads[num_threads];
    double total;
    int i, rc;
    scounter_t counter;
    init(&counter, threshold);
    args_t args[num_threads];
    struct timeval start, end;

    rc = gettimeofday(&start, NULL);
    if (rc != 0) {
        printf("gettimeofday error at start\n");
        exit(0);
    }

    for (i = 0; i < num_threads; i++) {
        args[i].mytid = i;      // thread ID
        args[i].loops = loops;
        args[i].counter = &counter;
        Pthread_create(&threads[i], NULL, ThreadCounter, &args[i]);
    }

    for (i = 0; i < num_threads; i++) {
        Pthread_join(threads[i], NULL);
    }

    rc = gettimeofday(&end, NULL);
    if (rc != 0) {
        printf("gettimeofday error at start\n");
        exit(0);
    }

    total = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    
    printf("%d threads incrementing a sloppy counter 1000000 times with %d thresholds took %f seconds.\n\n",num_threads, threshold, total / 1000000);

    printf("Counter Expected:\t%d\n\tActual: \t%d\n\n",loops * num_threads, get(&counter));

    return 0;

}