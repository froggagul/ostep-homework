# Chapter 28. Locks
## 1.
### 문제
Examine flag.s. This code “implements” locking with a single memory flag. Can you understand the assembly?
### 답안
```
.var flag
.var count

.main
.top

.acquire
mov  flag, %ax      # get flag
test $0, %ax        # if we get 0 back: lock is free!
jne  .acquire       # if not, try again
mov  $1, flag       # store 1 into flag

# critical section
mov  count, %ax     # get the value at the address
add  $1, %ax        # increment it
mov  %ax, count     # store it back

# release lock
mov  $0, flag       # clear the flag now

# see if we're still looping
sub  $1, %bx
test $0, %bx
jgt .top	

halt
```
* lock이 flag가 0이 될때까지 기다린다. 이후 critical section에 들어가기 전 flag에 1을 넣는다. critical section을 지난 후, flag에 0을 넣는다.

## 3.
### 문제
Change the value of the register %bx with the -a flag(e.g., -a bx=2,bx=2 if you are running just two threads). What does the code do? How does it change your answer for the question above?
### 답안
`python x86.py -p flag.s -M flag,count -R ax,bx -c -a bx=2,bx=2`
* 각 thread는 bx의 값인 2만큼 loop을 돈다. flag는 변하지 않는다.

## 5.
### 문제
Now let’s look at the program test-and-set.s. First, try to understand the code, which uses the xchg instruction to build a simple locking primitive. How is the lock acquire written? How about lock release?
### 답안
```
PS D:\study\operatingSystem\homework\threads-locks> python x86.py -p test-and-set.s -M mutex,count -R ax,bx -c
```
flag의 검사와 acquire이 한번에 이루어진다(atomic하게 이루어진다). 즉, 기존의 flag.s에서 발생할 수 있는 문제점이 test-and-set에서는 발생하지 않는다.

## 7.
### 문제
Use the -P flag to generate specific tests of the locking code. For example, run a schedule that grabs the lock in the first thread, but then tries to acquire it in the second. Does the right thing happen? What else should you test?
### 답안
`-P` flag를 사용해 first thread의 instruction 두번, second thread의 instruction을 두번 실행했다. Does the right thing happen?에 대한 답은 'Yes'이다. 아래 실행결과에서 알 수 있듯이, flag를 검사하는 과정 중간에 interupt가 일어나더라도, first thread에서 얻은 lock이 second thread로 갈 수 없다. 
```
$ python x86.py -p test-and-set.s -M mutex,count -R ax,bx -c -a bx=5,bx=5 -P 0011
mutex count      ax    bx          Thread 0                Thread 1
    0     0       0     5
    0     0       1     5   1000 mov  $1, %ax
    1     0       0     5   1001 xchg %ax, mutex
    1     0       0     5   ------ Interrupt ------  ------ Interrupt ------
    1     0       1     5                            1000 mov  $1, %ax
    1     0       1     5                            1001 xchg %ax, mutex
    1     0       0     5   ------ Interrupt ------  ------ Interrupt ------
    1     0       0     5   1002 test $0, %ax
    1     0       0     5   1003 jne  .acquire
    1     0       1     5   ------ Interrupt ------  ------ Interrupt ------
    1     0       1     5                            1002 test $0, %ax
    1     0       1     5                            1003 jne  .acquire
    1     0       0     5   ------ Interrupt ------  ------ Interrupt ------
    1     0       0     5   1004 mov  count, %ax
    1     0       1     5   1005 add  $1, %ax
    1     0       1     5   ------ Interrupt ------  ------ Interrupt ------
    1     0       1     5                            1000 mov  $1, %ax
    1     0       1     5                            1001 xchg %ax, mutex
    1     0       1     5   ------ Interrupt ------  ------ Interrupt ------
    1     1       1     5   1006 mov  %ax, count
    0     1       1     5   1007 mov  $0, mutex
    0     1       1     5   ------ Interrupt ------  ------ Interrupt ------
    0     1       1     5                            1002 test $0, %ax
    0     1       1     5                            1003 jne  .acquire
    0     1       1     5   ------ Interrupt ------  ------ Interrupt ------
    0     1       1     4   1008 sub  $1, %bx
    0     1       1     4   1009 test $0, %bx
    0     1       1     5   ------ Interrupt ------  ------ Interrupt ------
    0     1       1     5                            1000 mov  $1, %ax
    1     1       0     5                            1001 xchg %ax, mutex
    1     1       1     4   ------ Interrupt ------  ------ Interrupt ------
    1     1       1     4   1010 jgt .top
    1     1       1     4   1000 mov  $1, %ax
    1     1       0     5   ------ Interrupt ------  ------ Interrupt ------
    1     1       0     5                            1002 test $0, %ax
    1     1       0     5                            1003 jne  .acquire
    1     1       1     4   ------ Interrupt ------  ------ Interrupt ------
    1     1       1     4   1001 xchg %ax, mutex
    1     1       1     4   1002 test $0, %ax
    1     1       0     5   ------ Interrupt ------  ------ Interrupt ------
    1     1       1     5                            1004 mov  count, %ax
```
* 추가로는, test-and-set 기법에서 문제점으로 지적되는 공정성문제를 검사해야한다.
* 또, performance test도 수행해야 한다.
## 9.
### 문제
Now run the code with different values of -i. What kinds of different behavior do you see? Make sure to set the thread IDs appropriately (using -a bx=0,bx=1 for example) as the code assumes it.
### 답안
* 코드를 보면, 책에 있는 peterson의 알고리즘을 asm으로 옮긴 것이다.
```c++
int flag[2];
int turn;
void init() {
  flag[0] = flag[1] = 0; // 1−> 쓰레드가 락을 획득하길 원함
  turn = 0; // 누구의 차례인지를 나타내는 변수 turn
}
void lock() {
  flag[self] = 1; // self: 호출한 쓰레드의 ID
  turn = 1 − self; // 다른 쓰레드의 차례가 되도록 만든다.
  while((flag[1−self] == 1) && (turn == 1 − self))
  ; // 회전
}
void unlock() {
  flag[self] = 0; // 원래의 의도를 취소함
}
```
`python x86.py -p peterson.s -M count,flag,turn -R ax,cx -a bx=0,bx=1 -c -P 0011`을 실행함으로써 잘 작동함을 알 수 있었다.
```
```bash
count  flag  turn      ax    cx          Thread 0                Thread 1
    0     0     0       0     0
    0     0     0       0     0   1000 lea flag, %fx
    0     0     0       0     0   1001 mov %bx, %cx
    0     0     0       0     0   ------ Interrupt ------  ------ Interrupt ------
    0     0     0       0     0                            1000 lea flag, %fx
    0     0     0       0     1                            1001 mov %bx, %cx
    0     0     0       0     0   ------ Interrupt ------  ------ Interrupt ------
    0     0     0       0     0   1002 neg %cx
    0     0     0       0     1   1003 add $1, %cx
    0     0     0       0     1   ------ Interrupt ------  ------ Interrupt ------
    0     0     0       0    -1                            1002 neg %cx
    0     0     0       0     0                            1003 add $1, %cx
    0     0     0       0     1   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       0     1   1004 mov $1, 0(%fx,%bx,4)
    0     1     1       0     1   1005 mov %cx, turn
    0     1     1       0     0   ------ Interrupt ------  ------ Interrupt ------
    0     1     1       0     0                            1004 mov $1, 0(%fx,%bx,4)
    0     1     0       0     0                            1005 mov %cx, turn
    0     1     0       0     1   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       1     1   1006 mov 0(%fx,%cx,4), %ax
    0     1     0       1     1   1007 test $1, %ax
    0     1     0       0     0   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       1     0                            1006 mov 0(%fx,%cx,4), %ax
    0     1     0       1     0                            1007 test $1, %ax
    0     1     0       1     1   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       1     1   1008 jne .fini
    0     1     0       0     1   1009 mov turn, %ax
    0     1     0       1     0   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       1     0                            1008 jne .fini
    0     1     0       0     0                            1009 mov turn, %ax
    0     1     0       0     1   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       0     1   1010 test %cx, %ax
    0     1     0       0     1   1011 je .spin1
    0     1     0       0     0   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       0     0                            1010 test %cx, %ax
    0     1     0       0     0                            1011 je .spin1
    0     1     0       0     1   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       0     1   1012 mov count, %ax
    0     1     0       1     1   1013 add $1, %ax
    0     1     0       0     0   ------ Interrupt ------  ------ Interrupt ------
    0     1     0       1     0                            1006 mov 0(%fx,%cx,4), %ax
    0     1     0       1     0                            1007 test $1, %ax
    0     1     0       1     1   ------ Interrupt ------  ------ Interrupt ------
    1     1     0       1     1   1014 mov %ax, count
    1     0     0       1     1   1015 mov $0, 0(%fx,%bx,4)
    1     0     0       1     0   ------ Interrupt ------  ------ Interrupt ------
    1     0     0       1     0                            1008 jne .fini
    1     0     0       0     0                            1009 mov turn, %ax
    1     0     0       1     1   ------ Interrupt ------  ------ Interrupt ------
    1     0     1       1     1   1016 mov %cx, turn
    1     0     1       1     1   1017 halt
    1     0     1       0     0   ----- Halt;Switch -----  ----- Halt;Switch -----
    1     0     1       0     0                            1010 test %cx, %ax
    1     0     1       0     0                            1011 je .spin1
    1     0     1       0     0                            1006 mov 0(%fx,%cx,4), %ax
    1     0     1       0     0                            1007 test $1, %ax
    1     0     1       0     0                            1008 jne .fini
    1     0     1       1     0                            1012 mov count, %ax
    1     0     1       2     0                            1013 add $1, %ax
    2     0     1       2     0                            1014 mov %ax, count
    2     0     1       2     0                            1015 mov $0, 0(%fx,%bx,4)
    2     0     0       2     0                            1016 mov %cx, turn
    2     0     0       2     0                            1017 halt
```
