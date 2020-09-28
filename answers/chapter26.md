# Chapter 26. Concurrency: An Introduction
사용 코드: threads-intro/x86.py
## 1.
### 문제
> Let’s examine a simple program, “loop.s”. First, just read and understand it. Then, run it with these arguments (./x86.py -p loop.s -t 1 -i 100 -R dx) This specifies a single thread, an interrupt every 100 instructions, and tracing of register %dx. What will %dx be during the run? Use the -c flag to check your answers; the answers, on the left, show the value of the register (or memory value) after the instruction on the right has run.

> 우선 “loop.s”라는 간단한 프로그램을 먼저 살펴보자. cat loop.s 명령어로 프로그램을 먼저 읽어보고 이해할 수 있는지 보자. 그리고 다음의 인자 값을 사용하여
실행해 보자. 
`python x86.py −p loop.s −t 1 −i 100 −R dx`
위 명령어의 옵션은 한 개의 쓰레드, 100개의 명령어를 실행할 때마다 인터럽트 발생, %dx의 값을 추적하라는 옵션이다. 실행 중에 %dx의 값이 무엇일지 알 수 있겠는가? 그 값을 예상했다면, -c 플래그와 함께 다시 실행하여 답을 확인해 보자. 오른편의 명령어가 실행되면 왼편에 레지스터의 값 (또는 메모리 값)이 나타난다.
### 답안
```asm
.main
.top
sub  $1,%dx
test $0,%dx     
jgte .top         
halt
```
처음 dx가 intialize되지 않아 정확히 알수는 없지만, 0이라는 가정하면 sub 후 %dx는 -1이 될것이고 test 이후 0이 더 크므로 top으로 올라가지 않고 종료될 것이다.
```bash
   dx          Thread 0
    0
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
```
## 3.
### 문제
> Run this: ./x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx This makes the interrupt interval small/random; use different seeds (-s) to see different interleavings. Does the interrupt frequency change anything?

> 다음의 조건으로 실행해 보자.  
`./x86.py −p loop.s −t 2 −i 3 −r −a dx=3,dx=3 −R dx`
위의 명령어는 인터럽트가 짧은 간격으로 아무 때나 발생하도록 지정한다. 다른 순서의 쓰레드 실행을 보고 싶다면 -s 옵션으로 새로운 시드를 사용하라. 인터럽트 실행 빈도를 변경하는 것이 프로그램의 결과에 영향을 주는가?
### 답안
* edx의 값이 thread별로 지정되어있기 때문에, interruption이 얼마나 많이 일어나는지에 상관없이, 프로그램은 4번 순환한다.
```
PS D:\study\operatingSystem\homework\threads-intro> python x86.py -p loop.s -t 2 -i 3 -r -a dx=3,dx=3 -R dx -c
   dx          Thread 0                Thread 1
    3
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    3   ------ Interrupt ------  ------ Interrupt ------
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    2   ------ Interrupt ------  ------ Interrupt ------
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    2   ------ Interrupt ------  ------ Interrupt ------
    1                            1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
    1   ------ Interrupt ------  ------ Interrupt ------
    0                            1000 sub  $1,%dx
   -1   ------ Interrupt ------  ------ Interrupt ------
   -1   1001 test $0,%dx
   -1   1002 jgte .top
    0   ------ Interrupt ------  ------ Interrupt ------
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1   ------ Interrupt ------  ------ Interrupt ------
   -1   1003 halt
    0   ----- Halt;Switch -----  ----- Halt;Switch -----
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1   ------ Interrupt ------  ------ Interrupt ------
   -1                            1002 jgte .top
   -1                            1003 halt
```
```
PS D:\study\operatingSystem\homework\threads-intro> python x86.py -p loop.s -t 2 -i 100 -r -a dx=3,dx=3 -R dx -c
   dx          Thread 0                Thread 1
    3
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgte .top
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    1   1002 jgte .top
    0   1000 sub  $1,%dx
    0   1001 test $0,%dx
    0   1002 jgte .top
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgte .top
   -1   1003 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgte .top
    1                            1000 sub  $1,%dx
    1                            1001 test $0,%dx
    1                            1002 jgte .top
    0                            1000 sub  $1,%dx
    0                            1001 test $0,%dx
    0                            1002 jgte .top
   -1                            1000 sub  $1,%dx
   -1                            1001 test $0,%dx
   -1                            1002 jgte .top
   -1                            1003 halt
```
## 6.
### 문제
> Run with random interrupt intervals: ./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0 with different seeds (-s 1, -s 2, etc.) Can you tell by looking at the thread interleaving what the final value of value will be? Does the timing of the interrupt matter? Where can it safely occur? Where not? In other words, where is the critical section exactly

> 랜덤 간격으로 인터럽트가 발생하도록 하여 실행해 보자.  
`python x86.py -p looping−race−nolock.s -t 2 -M 2000 -i 4 -r -s 0`
실행한 후에 랜덤 함수의 시드를 -s 1 로 해보고 -s 2 등으로 변경해 보자. 쓰레드 실행 순서를 보고 x의 최종 값이 무ᨨ인지 알 수 있겠는가? 인터럽트의 정확한 발생 위치가 중요한가? 인터럽트가 발생해도 안전한 위치는 어디인가? 인터럽트가 문제를 일으키는 위치는 어디인가? 다시 말해 정확하게 어디가 임계 영역인가?
### 답안
`looping-race-nolock.s`는 두가지 부분으로 나누어져 있다. address 2000의 값을 수정하는 부분과, %bx의 값을 수정하고 test하는 부분이다. %bx의 값을 수정하고 test하는 부분은 3번과 같이 각 thread에 저장되기 때문에, intterupt가 일어나도 결과가 달라지지 않는다. 그러나, address 2000에 값을 수정하는 부분은 하나의 공통된 주소 값에 대해 연산을 하기 때문에, interrupt가 일어나는 위치에 따라 결과가 달라질 수 있다.  
예를들어, 아레의 예시처럼 각 thread에서 mov 2000, %ax 다음에서 바로 interrupt가 일어나는 경우 두 thread의 %ax에 0이 들어가기 때문에, 최종적으로는 2000의 주소에 1이 들어간다.
```bash
 2000          Thread 0                Thread 1
    0
    0   1000 mov 2000, %ax
    0   ------ Interrupt ------  ------ Interrupt ------
    0                            1000 mov 2000, %ax
    0   ------ Interrupt ------  ------ Interrupt ------
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   ------ Interrupt ------  ------ Interrupt ------
    1                            1001 add $1, %ax
    1   ------ Interrupt ------  ------ Interrupt ------
```
그렇기 때문에 코드에서 critical section은 mov 2000, %ax부터 mov %ax, 2000까지이다.

## 9.
### 문제
> One last program: wait-for-me.s. Run: ./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000 This sets the %ax register to 1 for thread 0, and 0 for thread 1, and watches %ax and memory location 2000. How should the code behave? How is the value at location 2000 being used by the threads? What will its final value be?

> 이 숙제의 마지막으로 wait-for-me.s 프로그램을 살펴보려고 한다. 다음과 같이 코드를 실행해 보자.
`python x86.py -p wait-for-me.s -a ax=1, ax=0 -R ax -M 2000`
쓰레드 0의 %ax 레지스터는 1로 설정하고 쓰레드 1의 레지스터는 0으로 설정한 후에 프로그램이 실행하는 동안 %ax의 값과 메모리 2000번지에 있는 값을 주시하라. 코드는 어떤 행동을 보여야 하는가? 쓰레드들은 2000번지의 값을 어떻게 사용하는가? 최종 값은 무엇이 되겠는가?
### 답안
쓰레드 1은 signaller, 쓰레드 2는 waiter쪽 코드를 사용한다. 이때 signaller인 쓰레드1이 먼저 실행되기 때문에 address 2000에 1 값을 넣는다.  
이후 쓰레드2가 address 2000에서 값을 불러와 1인지, 0인지를 test한다. 이때, 값이 signaller에 의해 먼저 값이 1로 변경되었으므로, 기다리지 않고 바로 halt된다. 최종 값으로는 address 2000에 1이 남아있을 것이다. 실제 코드 실행 결과는 다음과 같다.
```
 2000      ax    cx          Thread 0                Thread 1
    0       1     0
    0       1     0   1000 test $1, %ax
    0       1     0   1001 je .signaller
    1       1     0   1006 mov  $1, 2000
    1       1     0   1007 halt
    1       0     0   ----- Halt;Switch -----  ----- Halt;Switch -----
    1       0     0                            1000 test $1, %ax
    1       0     0                            1001 je .signaller
    1       0     1                            1002 mov  2000, %cx
    1       0     1                            1003 test $1, %cx
    1       0     1                            1004 jne .waiter
    1       0     1                            1005 halt
```