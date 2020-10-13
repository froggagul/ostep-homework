# Chapter 8. Scheduling: The Multi-Level Feedback Queue
사용 코드는 다음과 같다
* cpu-sched-mlfq/mlfq.py
## 1.
### 질문
Run a few randomly-generated problems with just two jobs and two queues; compute the MLFQ execution trace for each. Make your life easier by limiting the length of each job and **turning off I/Os.**
### 답안
IO가 꺼져있으므로 길이에 상관없이 첫번째 job이 실행된 후, 두번째 job이 실행될 것이다.
최대 길이를 10으로 하고 결과를 측정했다.
```
python mlfq.py -s 1 -n 2 -j 2 -m 10 -M 0 -c
[ time 0 ] JOB BEGINS by JOB 0
[ time 0 ] JOB BEGINS by JOB 1
[ time 0 ] Run JOB 0 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 1 (of 2) ]
[ time 1 ] Run JOB 0 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 0 (of 2) ]
[ time 2 ] FINISHED JOB 0
[ time 2 ] Run JOB 1 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 6 (of 7) ]
[ time 3 ] Run JOB 1 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 5 (of 7) ]
[ time 4 ] Run JOB 1 at PRIORITY 1 [ TICKS 7 ALLOT 1 TIME 4 (of 7) ]
[ time 5 ] Run JOB 1 at PRIORITY 1 [ TICKS 6 ALLOT 1 TIME 3 (of 7) ]
[ time 6 ] Run JOB 1 at PRIORITY 1 [ TICKS 5 ALLOT 1 TIME 2 (of 7) ]
[ time 7 ] Run JOB 1 at PRIORITY 1 [ TICKS 4 ALLOT 1 TIME 1 (of 7) ]
[ time 8 ] Run JOB 1 at PRIORITY 1 [ TICKS 3 ALLOT 1 TIME 0 (of 7) ]
[ time 9 ] FINISHED JOB 1
```
```
python mlfq.py -s 0 -n 2 -j 2 -m 10 -M 0 -c
[ time 0 ] JOB BEGINS by JOB 0
[ time 0 ] JOB BEGINS by JOB 1
[ time 0 ] Run JOB 0 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 7 (of 8) ]
[ time 1 ] Run JOB 0 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 6 (of 8) ]
[ time 2 ] Run JOB 0 at PRIORITY 1 [ TICKS 7 ALLOT 1 TIME 5 (of 8) ]
[ time 3 ] Run JOB 0 at PRIORITY 1 [ TICKS 6 ALLOT 1 TIME 4 (of 8) ]
[ time 4 ] Run JOB 0 at PRIORITY 1 [ TICKS 5 ALLOT 1 TIME 3 (of 8) ]
[ time 5 ] Run JOB 0 at PRIORITY 1 [ TICKS 4 ALLOT 1 TIME 2 (of 8) ]
[ time 6 ] Run JOB 0 at PRIORITY 1 [ TICKS 3 ALLOT 1 TIME 1 (of 8) ]
[ time 7 ] Run JOB 0 at PRIORITY 1 [ TICKS 2 ALLOT 1 TIME 0 (of 8) ]
[ time 8 ] FINISHED JOB 0
[ time 8 ] Run JOB 1 at PRIORITY 1 [ TICKS 9 ALLOT 1 TIME 3 (of 4) ]
[ time 9 ] Run JOB 1 at PRIORITY 1 [ TICKS 8 ALLOT 1 TIME 2 (of 4) ]
[ time 10 ] Run JOB 1 at PRIORITY 1 [ TICKS 7 ALLOT 1 TIME 1 (of 4) ]
[ time 11 ] Run JOB 1 at PRIORITY 1 [ TICKS 6 ALLOT 1 TIME 0 (of 4) ]
[ time 12 ] FINISHED JOB 1
```
## 2.
### 질문
How would you run the scheduler to reproduce each of the examples in the chapter?
### 답안
각 명령어를 실행함으로써 구현할 수 있다.
1. Figure 8.2 Long-running Job Over Time
    ```bash
    python mlfq.py -n 3 -q 10 -l 0,200,0 -c
    ```
2. Figure 8.3 Along Came An Interactive Job
    ```bash
    python mlfq.py -n 3 -q 10 -l 0,180,0:100,20,0 -c
    ```
3. Figure 8.4 A Mixed I/O-intensive and CPU-intensive Workload
    ```bash
    python mlfq.py -n 3 -q 10 -l 0,175,0:50,25,1 -i 5 -S -c
    ```
4. Figure 8.5 without priority boost
    ```bash
    python mlfq.py -n 3 -q 10 -l 0,120,0:100,50,1:100,50,1 -i 1 -S -c
    ```
5. Figure 8.5 with priority boost
    ```bash
    python mlfq.py -n 3 -q 10 -l 0,120,0:100,50,1:100,50,1 -i 1 -S -B 50 -c
    ```
6. Figure 8.6 without gaming tolerance
    ```bash
    python mlfq.py -n 3 -q 10 -i 1 -S -l 0,200,0:80,100,9 -c
    ```
7. Figure 8.6 with gaming tolerance
    ```bash
    python mlfq.py -n 3 -q 10 -i 1 -l 0,200,0:80,100,9 -c
    ```
8. Figure 8.7 Lower Priority, Longer Quanta
    ```bash
    python mlfq.py -n 3 -a 2 -Q 10,20,40 -l 0,200,0:0,200,0 -c
    ```
## 3.
### 질문
How would you configure the scheduler parameters to behave just like a round-robin scheduler?
### 답안
큐의 개수가 하나만 있고 time-slice가 동일하면, 단일 큐에서 동작하는 round-robin scheduler와 동일하게 작동한다.