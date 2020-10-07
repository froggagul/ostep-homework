# 29. Lock-based Concurrent Data Structures
## 1.
### 문제
> We’ll start by redoing the measurements within this chapter. Use the call gettimeofday() to measure time within your program. How accurate is this timer? What is the smallest interval it can measure? Gain confidence in its workings, as we will need it in all subsequent questions. You can also look into other timers, such as the cycle counter available on x86 via the rdtsc instruction.
### 답안
```
#include <sys/time.h>
#include <stdio.h>
int main() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  printf("초 : %ld\n마이크로 초 : %ld", current_time.tv_sec, current_time.tv_usec);
  return 0;
}
```
* microsecond까지 측정가능하다.
## 2.
### 문제
Now, build a simple concurrent counter and measure how long it takes to increment the counter many times as the number of threads increases. How many CPUs are available on the system you are using? Does this number impact your measurements at all?
### 답안
```bash
froggagul@ubuntu:~/Desktop$ grep -c processor /proc/cpuinfo
4
```
```c++
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
```
```bash
froggagul@ubuntu:~/ostep/answers$ ./simple 1
1threads with 100000 loops took 0.028860 seconds

Counter Expected: 	1000000
Actual: counter_get: 	1000000
froggagul@ubuntu:~/ostep/answers$ ./simple 2
2threads with 100000 loops took 0.156991 seconds

Counter Expected: 	2000000
Actual: counter_get: 	2000000
froggagul@ubuntu:~/ostep/answers$ ./simple 3
3threads with 100000 loops took 0.261670 seconds

Counter Expected: 	3000000
Actual: counter_get: 	3000000
froggagul@ubuntu:~/ostep/answers$ ./simple 4
4threads with 100000 loops took 0.349335 seconds

Counter Expected: 	4000000
Actual: counter_get: 	4000000
```
| 1     | 2     | 3     | 4     |
|-------|-------|-------|-------|
| 0.029 | 0.157 | 0.262 | 0.349 |
* 책의 결과와 비슷하게 나왔다. thread가 증가할수록 시간이 증가한다.

## 3.
### 문제
Next, build a version of the sloppy counter. Once again, measure its performance as the number of threads varies, as well as the threshold. Do the numbers match what you see in the chapter?
### 답안
다음의 코드를 사용하였다.
```c++
froggagul@ubuntu:~/ostep/answers$ cat sloppy_counter.c
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
```
```
froggagul@ubuntu:~/ostep/answers$ gcc -pthread -o sloppy_counter sloppy_counter.c 
froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 1
Number of threads:	4
Size of threshold:	1
4 threads incrementing a sloppy counter 1000000 times with 1 thresholds took 0.646247 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 2
Number of threads:	4
Size of threshold:	2
4 threads incrementing a sloppy counter 1000000 times with 2 thresholds took 0.451433 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 4
Number of threads:	4
Size of threshold:	4
4 threads incrementing a sloppy counter 1000000 times with 4 thresholds took 0.424433 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 8
Number of threads:	4
Size of threshold:	8
4 threads incrementing a sloppy counter 1000000 times with 8 thresholds took 0.349734 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 16
Number of threads:	4
Size of threshold:	16
4 threads incrementing a sloppy counter 1000000 times with 16 thresholds took 0.360983 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 32
Number of threads:	4
Size of threshold:	32
4 threads incrementing a sloppy counter 1000000 times with 32 thresholds took 0.310834 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 64
Number of threads:	4
Size of threshold:	64
4 threads incrementing a sloppy counter 1000000 times with 64 thresholds took 0.267567 seconds.

Counter Expected:	4000000
	Actual: 	4000000

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 128
Number of threads:	4
Size of threshold:	128
4 threads incrementing a sloppy counter 1000000 times with 128 thresholds took 0.232917 seconds.

Counter Expected:	4000000
	Actual: 	3999744

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 256
Number of threads:	4
Size of threshold:	256
4 threads incrementing a sloppy counter 1000000 times with 256 thresholds took 0.274468 seconds.

Counter Expected:	4000000
	Actual: 	3999744

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 512
Number of threads:	4
Size of threshold:	512
4 threads incrementing a sloppy counter 1000000 times with 512 thresholds took 0.286110 seconds.

Counter Expected:	4000000
	Actual: 	3999744

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 1024
Number of threads:	4
Size of threshold:	1024
4 threads incrementing a sloppy counter 1000000 times with 1024 thresholds took 0.293305 seconds.

Counter Expected:	4000000
	Actual: 	3997696

froggagul@ubuntu:~/ostep/answers$ ./sloppy_counter 4 2048
Number of threads:	4
Size of threshold:	2048
4 threads incrementing a sloppy counter 1000000 times with 2048 thresholds took 0.277229 seconds.

Counter Expected:	4000000
	Actual: 	3997696
```
| 1     | 2     | 4     | 8     | 16    | 32    | 64    | 128   | 256   | 512   | 1024  | 2048  |
|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
| 0.646 | 0.451 | 0.424 | 0.350 | 0.361 | 0.311 | 0.268 | 0.233 | 0.274 | 0.286 | 0.293 | 0.277 |
| 0     | 0     | 0     | 0     | 0     | 0     | 0     | 256   | 256   | 256   | 2304  | 2304  |
* threshold가 증가할수록 손실률을 증가하는 대신, count하는 시간이 줄어든다.