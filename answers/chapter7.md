# Chapter 7. Scheduling: Introduction
사용 코드는 다음과 같다
* cpu-sched/scheduler.py
## 1.
### 질문
Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.
### 답안
job의 length가 200으로 모두 동일하므로, SJF와 FIFO scheduler의 average response time이 동일하다.
```sh
PS D:\study\operatingSystem\homework\cpu-sched> python ./scheduler.py -p SJF -l 200,200,200 -c
ARG policy SJF
ARG jlist 200,200,200
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 200.00  Wait 0.00
  Job   1 -- Response: 200.00  Turnaround 400.00  Wait 200.00
  Job   2 -- Response: 400.00  Turnaround 600.00  Wait 400.00

  Average -- Response: 200.00  Turnaround 400.00  Wait 200.00
```
```
PS D:\study\operatingSystem\homework\cpu-sched> python ./scheduler.py -p FIFO -l 200,200,200 -c
ARG policy FIFO
ARG jlist 200,200,200
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 200.00  Wait 0.00
  Job   1 -- Response: 200.00  Turnaround 400.00  Wait 200.00
  Job   2 -- Response: 400.00  Turnaround 600.00  Wait 400.00

  Average -- Response: 200.00  Turnaround 400.00  Wait 200.00
```
## 2.
### 질문
Now do the same but with jobs of different lengths: 100, 200, and 300.
### 답안
FIFO이건 SFJ이건 job의 길이가 모두 같으므로, job의 길이가 줄어듬에 따라 average response time은 줄어들지만 순서는 바뀌지 않는다. 실행 결과는 job의 길이가 100일 때만을 첨부하였다.
```
PS D:\study\operatingSystem\homework\cpu-sched> python ./scheduler.py -p SJF -l 100,100,100 -c
ARG policy SJF
ARG jlist 100,100,100
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   1 -- Response: 100.00  Turnaround 200.00  Wait 100.00
  Job   2 -- Response: 200.00  Turnaround 300.00  Wait 200.00

  Average -- Response: 100.00  Turnaround 200.00  Wait 100.00

PS D:\study\operatingSystem\homework\cpu-sched> python ./scheduler.py -p FIFO -l 100,100,100 -c
ARG policy FIFO
ARG jlist 100,100,100
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   1 -- Response: 100.00  Turnaround 200.00  Wait 100.00
  Job   2 -- Response: 200.00  Turnaround 300.00  Wait 200.00

  Average -- Response: 100.00  Turnaround 200.00  Wait 100.00
```
## 3.
### 질문
Now do the same, but also with the RR scheduler and a time-slice of 1.
### 답안
average response time이 1로 줄어들었다
```
PS D:\study\operatingSystem\homework\cpu-sched> python scheduler.py -p RR -q 1 -l 100,100,100 -c
ARG policy RR
ARG jlist 100,100,100

Here is the job list, with the run time of each job:
  Job 0 ( length = 100.0 )
  Job 1 ( length = 100.0 )
  Job 2 ( length = 100.0 )
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 298.00  Wait 198.00
  Job   1 -- Response: 1.00  Turnaround 299.00  Wait 199.00
  Job   2 -- Response: 2.00  Turnaround 300.00  Wait 200.00

  Average -- Response: 1.00  Turnaround 299.00  Wait 199.00
```

## 4.
### 질문
For what types of workloads does SJF deliver the same turnaround times as FIFO?
### 답안
FIFO는 job이 들어오는 순서대로 실행 순서를 결정하는데, 각 job의 length가 작은것부터 먼저 순서로 주어지는 경우, SJF와 같다. 먼저 오는 job이 실행시간도 빠르기 때문이다.
```
PS D:\study\operatingSystem\homework\cpu-sched> python ./scheduler.py -p FIFO -l 1,10,100 -c
ARG policy FIFO
ARG jlist 1,10,100
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 1.00  Wait 0.00
  Job   1 -- Response: 1.00  Turnaround 11.00  Wait 1.00
  Job   2 -- Response: 11.00  Turnaround 111.00  Wait 11.00

  Average -- Response: 4.00  Turnaround 41.00  Wait 4.00

PS D:\study\operatingSystem\homework\cpu-sched> python ./scheduler.py -p SJF -l 1,10,100 -c
ARG policy SJF
ARG jlist 1,10,100
(중략)
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 1.00  Wait 0.00
  Job   1 -- Response: 1.00  Turnaround 11.00  Wait 1.00
  Job   2 -- Response: 11.00  Turnaround 111.00  Wait 11.00

  Average -- Response: 4.00  Turnaround 41.00  Wait 4.00
```