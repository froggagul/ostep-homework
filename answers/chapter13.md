# Chapter 13. The Abstraction: Address Spaces
## 3. 
Next, create a little program that uses a certain amount of memory, called memory-user.c. This program should take one command-line argument: the number of megabytes of memory it will use. When run, it should allocate an array, and constantly stream through the array, touching each entry. The program should do this indefinitely, or, perhaps, for a certain amount of time also specified at the command line.
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int *arr;
int runTime;

void sigint_handler(int signo)
{
   printf("alarm went off in %d seconds\n", runTime);
   free(arr);
   exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: memory-user <memory> <time>\n");
        exit(EXIT_FAILURE);
    }

    printf("pid: %d\n", getpid());
    // get memory and runTime
    int memory = atoi(argv[1]) * 1024 * 1024;
    int length = (int)(memory / sizeof(int));
    runTime = atoi(argv[2]);
    // alarm and malloc
    signal(SIGALRM, sigint_handler);
    alarm(runTime);
    arr = malloc(memory);

    while (1)
    {
        for (int i = 0; i < length; i++)
            arr[i] += 1;
    }

    
    return 0;
}
```
## 4. 
Now, while running your memory-user program, also (in a different terminal window, but on the same machine) run the free tool. How do the memory usage totals change when your program is running? How about when you kill the memory-user program? Do the numbers match your expectations? Try this for different amounts of memory usage. What happens when you use really large amounts of memory?
```bash
froggagul@ubuntu:~/ostep/answers/13$ free
              total        used        free      shared  buff/cache   available
Mem:        8124868     1326848     5402572       43512     1395448     6507584
Swap:        945416           0      945416
froggagul@ubuntu:~/ostep/answers/13$ ./memory-user 900000 10
pid: 3995
alarm went off in 10 seconds
froggagul@ubuntu:~/ostep/answers/13$ free
              total        used        free      shared  buff/cache   available
Mem:        8124868     1336092     5393224       43512     1395552     6498324
Swap:        945416           0      945416
froggagul@ubuntu:~/ostep/answers/13$ ./memory-user 900000 10
pid: 4049
^C
froggagul@ubuntu:~/ostep/answers/13$ free
              total        used        free      shared  buff/cache   available
Mem:        8124868     1325012     5405128       42488     1394728     6510404
Swap:        945416           0      945416
```
* 다른 프로세스의 영향으로 인해 측정할 수 없었다.

## 6.
To use pmap, you have to know the process ID of the process you’re interested in. Thus, first run ps auxw to see a list of all processes; then, pick an interesting one, such as a browser. You can also use your memory-user program in this case (indeed, you can even have that program call getpid() and print out its PID for your convenience).
```
2586:   ./memory-user 900000 10
00005626be511000      4K r---- memory-user
00005626be512000      4K r-x-- memory-user
00005626be513000      4K r---- memory-user
00005626be514000      4K r---- memory-user
00005626be515000      4K rw--- memory-user
00005626beb7c000    132K rw---   [ anon ]
00007f9aeb777000    148K r---- libc-2.31.so
00007f9aeb79c000   1504K r-x-- libc-2.31.so
00007f9aeb914000    296K r---- libc-2.31.so
00007f9aeb95e000      4K ----- libc-2.31.so
00007f9aeb95f000     12K r---- libc-2.31.so
00007f9aeb962000     12K rw--- libc-2.31.so
00007f9aeb965000     24K rw---   [ anon ]
00007f9aeb97d000      4K r---- ld-2.31.so
00007f9aeb97e000    140K r-x-- ld-2.31.so
00007f9aeb9a1000     32K r---- ld-2.31.so
00007f9aeb9aa000      4K r---- ld-2.31.so
00007f9aeb9ab000      4K rw--- ld-2.31.so
00007f9aeb9ac000      4K rw---   [ anon ]
00007fffcd962000    132K rw---   [ stack ]
00007fffcd995000     12K r----   [ anon ]
00007fffcd998000      4K r-x--   [ anon ]
ffffffffff600000      4K --x--   [ anon ]
 total             2492K
```

## 7.
Now run pmap on some of these processes, using various flags (like -X) to reveal many details about the process. What do you see? How many different entities make up a modern address space, as opposed to our simple conception of code/stack/heap?
### 답안
* 프로세스의 메모리맵을 볼 수 있다.
* libc라는 이름으로 C libraries들이 memory를 구성한다.