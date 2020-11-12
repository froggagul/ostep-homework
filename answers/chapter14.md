# Chapter 14. Interlude: Memory API
## 1.
First, write a simple program called null.c that creates a pointer to an integer, sets it to NULL, and then tries to dereference it. Compile this into an executable called null. What happens when you run this program?  
null.c
```c++
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int *x = (int *) malloc(1);
    x = NULL;
    printf("%d\n", *x);
    free(x);
    return 0;
}
```
```bash
froggagul@ubuntu:~/ostep/answers/14$ gcc null.c -o null
froggagul@ubuntu:~/ostep/answers/14$ ./null
Segmentation fault (core dumped)
```
* segmentation fauilt 발생.
## 2.
Next,compile this program with symbol information included(with the -g flag). Doing so let’s put more information into the executable, enabling the debugger to access more useful information about variable names and the like. Run the program under the debugger by typing `gdb null` and then, once gdb is running, typing run. What does gdb show you?
```bash
froggagul@ubuntu:~/ostep/answers/14$ gdb null
...
gdb-peda$ r
Starting program: /home/froggagul/ostep/answers/14/null 
...
Stopped reason: SIGSEGV
0x00005555555551b6 in main ()
```
## 3.
Finally, use the valgrind tool on this program. We’ll use the memcheck tool that is a part of valgrind to analyze what happens. Run this by typing in the following: `valgrind --leak-check=yes null`. What happens when you run this? Can you interpret the output from the tool?
```
froggagul@ubuntu:~/ostep/answers/14$ valgrind --leak-check=yes ./null
(중략)
==3343==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==3343== 
==3343== 
==3343== Process terminating with default action of signal 11 (SIGSEGV)
==3343==  Access not within mapped region at address 0x0
==3343==    at 0x1091B6: main (in /home/froggagul/ostep/answers/14/null)
(중략)
Segmentation fault (core dumped)
```
* x 는 0x0의 주소에 있고, program 상의 주소가 아니다.

## 4.
Write a simple program that allocates memory using malloc() but forgets to free it before exiting. What happens when this program runs? Can you use gdb to find any problems with it? How about valgrind (again with the --leak-check=yes flag)?
* 실행시 문제가 발생하지 않는다.
* gdb또한 마찬가지이다.
* valgrind로 검사시 lost된 1개의 block를 지칭해준다. (1개의 error)
```
==3425== HEAP SUMMARY:
==3425==     in use at exit: 4 bytes in 1 blocks
==3425==   total heap usage: 2 allocs, 1 frees, 1,028 bytes allocated
==3425== 
==3425== 4 bytes in 1 blocks are definitely lost in loss record 1 of 1
==3425==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==3425==    by 0x10917E: main (in /home/froggagul/ostep/answers/14/forget_free)
==3425== 
==3425== LEAK SUMMARY:
==3425==    definitely lost: 4 bytes in 1 blocks
==3425==    indirectly lost: 0 bytes in 0 blocks
==3425==      possibly lost: 0 bytes in 0 blocks
==3425==    still reachable: 0 bytes in 0 blocks
==3425==         suppressed: 0 bytes in 0 blocks
==3425== 
==3425== For lists of detected and suppressed errors, rerun with: -s
```