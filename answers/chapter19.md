# Chapter 19. Paging: Introduction
## 1.
For timing, you’ll need to use a timer (e.g., gettimeofday()). How precise is such a timer? How long does an operation have to take in order for you to time it precisely? (this will help determine how many times, in a loop, you’ll have to repeat a page access in order to time it successfully)
### Answer
* `time()`를 사용하게 되면 `time()`의 최소단위인 operation이 1초보다 길게 실행되어야 한다
    > time() returns the time as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
    * 출처: https://man7.org/linux/man-pages/man2/time.2.html
* 다른 timer를 사용할때도 그 timer의 최소단위보다 operation이 길게 수행되어야 한다.
    * 정확한 측정을 위해서는 측정 단위가 작은 library를 사용해야 한다.
    * 프로그램 구현때는 times(2)를 사용했다. 
    >   The number of clock ticks per second can be obtained using: `sysconf(_SC_CLK_TCK);`
    * https://man7.org/linux/man-pages/man2/times.2.html

## 2.
Write the program, called tlb.c, that can roughly measure the cost of accessing each page. Inputs to the program should be: the number of pages to touch and the number of trials.
### Answer
tlb.c는 다음과 같다.
```c++
#include <errno.h>
#include <pthread.h>   // pthread_self
#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // exit, malloc, free
#include <sys/times.h> // times
#include <unistd.h>    // sysconf

int main(int argc, char *argv[]) {

  long PAGESIZE = sysconf(_SC_PAGESIZE);
  long clktck = sysconf(_SC_CLK_TCK);
  long jump = PAGESIZE / (long)sizeof(int);
  int NUMPAGES = atoi(argv[1]);
  int trails = atoi(argv[2]);

  int *a = (int *)malloc((size_t)NUMPAGES * (size_t)PAGESIZE);
  struct tms tmsstart, tmsend;
  clock_t start, end;
  // start time
  start = times(&tmsstart);

  for (int j = 0; j < trails; j++) {
    for (int i = 0; i < NUMPAGES * jump; i += jump) {
      a[i] += 1;
    }
  }
  // end time
  end = times(&tmsend);

  int nloops = trails * NUMPAGES;
  // nanoseconds
  printf("%f %f %f\n", (double)(end - start) / (double)clktck * 1000000000 / nloops,
         ((double)(tmsend.tms_utime - tmsstart.tms_utime) / (double)clktck) *
             1000000000 / nloops,
         ((double)(tmsend.tms_stime - tmsstart.tms_stime) / (double)clktck) *
             1000000000 / nloops);
  free(a);
  return 0;
}
```

## 3.
Now write a script in your favorite scripting language (bash?) to run this program, while varying the number of pages accessed from 1 up to a few thousand, perhaps incrementing by a factor of two per iteration. Run the script on different machines and gather some data. How many trials are needed to get reliable measurements?
### Answer
* 1microsecond까지 측정 가능하므로, 한 페이지를 접하는데 1 nanosecond가 든다고 가정하면, trials는 100 이상이어야 한다.