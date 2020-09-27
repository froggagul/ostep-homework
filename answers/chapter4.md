# chapter4
사용 코드는 다음과 같다
* cpu-intro/process-run.py
## 1.
### 질문
> Run process-run.py with the following flags: -l 5:100,5:100. What should the CPU utilization be (e.g., the percent of time the CPU is in use?) Why do you know this? Use the -c and -p flags to see if you were right.
> process-run.py를 다음 flag와 함께 돌리세요 `-l 5:100,5:100`. [CPU 사용률](https://ko.wikipedia.org/wiki/CPU_%ED%83%80%EC%9E%84)은 어떻게 될 것인가요? (예: 전체 시간중 CPU를 사용하는 시간의 비율) 그리고 그것의 근거는 무엇인가요? -c, -p flag를 사용해 맞는지 확인해 보세요
### 답안
* 우리가 선언한 process-list는 "5:100, 5:100"이다. 이는 process가 각각 5개의 명령어로 구성되어 있으며, CPU 명령어일 가능성은 100%이다.
```
PS D:\study\operatingSystem\homework\cpu-intro> python process-run.py -l 5:100,5:100 
Produce a trace of what would happen when you run these processes:
Process 0
  cpu
  cpu
  cpu
  cpu
  cpu

Process 1
  cpu
  cpu
  cpu
  cpu
  cpu

Important behaviors:
  System will switch whenthe current process is FINISHED or ISSUES AN IO
  After IOs, the process issuing the IO willrun LATER (when it is its turn)
```
* cpu에서 모두 instruction이 실행되어야 하므로, 한 프로세스에서 모두 명령어를 실행하고 난 뒤에, 두번째 프로세스에서 명령어를 실행할 것이다.
* 이는 cpu 사용률이 100%임을 의미하기도 한다. (언제나 cpu가 둘중 하나의 프로세스에 의해 사용되므로)

-c, -p flag를 넣어 결과를 확인해 보았다.
```bash
PS D:\study\operatingSystem\homework\cpu-intro> python process-run.py -l 5:100,5:100 -c -p
Time    PID: 0    PID: 1       CPU       IOs
  1    RUN:cpu     READY         1
  2    RUN:cpu     READY         1
  3    RUN:cpu     READY         1
  4    RUN:cpu     READY         1
  5    RUN:cpu     READY         1
  6       DONE   RUN:cpu         1
  7       DONE   RUN:cpu         1
  8       DONE   RUN:cpu         1
  9       DONE   RUN:cpu         1
 10       DONE   RUN:cpu         1

Stats: Total Time 10
Stats: CPU Busy 10 (100.00%)
Stats: IO Busy  0 (0.00%)
```

## 3.
### 문제
> Switch the order of the processes: -l 1:0,4:100. What happens now? Does switching the order matter? Why? (As always, use -c and -p to see if you were right)
> 프로세스의 순서를 `-l 1:0,4:100`로 바꿔보세요. 무슨일이 일어나나요? 이 순서를 바꾸는 것이 중요한가요? 왜 그런가요? (언제나 그렇듯이, `-c`, `-p`를 사용하여 답을 확인하세요)

### 답안
* 하나의 프로세스는 IO만을, 하나의 프로세스는 cpu instruction만을 수행한다. 우선 `process-run.py -; 1:0,4:100`을 실행하면 다음과 같은 문구를 볼 수 있다
```bash
Important behaviors:
  System will switch when the current process is FINISHED or ISSUES AN IO
  After IOs, the process issuing the IO will run LATER (when it is its turn)
(해석)
중요한 행동들:
  시스템은 process가 끝나거나, IO를 하겠다고 선언(issue)할때 switch합니다.
  IO가 끝나면, IO를 하겠다고 선언한 process는 나중에 실행됩니다(turn이 오면)
```
* 처음 오는 프로세스의 명령어인 IO를 먼저 선언하고, 그동안 cpu 명령어로만 이루어진 process가 돌아갈 것이다. 

```bash
PS D:\study\operatingSystem\homework\cpu-intro> python process-run.py -l 1:0,4:100 -c -p
Time    PID: 0    PID: 1       CPU       IOs
  1     RUN:io     READY         1
  2    WAITING   RUN:cpu         1         1
  3    WAITING   RUN:cpu         1         1
  4    WAITING   RUN:cpu         1         1
  5    WAITING   RUN:cpu         1         1
  6*      DONE      DONE         1

Stats: Total Time 6
Stats: CPU Busy 6 (100.00%)
Stats: IO Busy  4 (66.67%)
```
* 만약 `process-run.py -l 4:100,1:0`를 실행했다면, 4개의 cpu instruciton으로 구성되있는 첫번째 process가 먼저 실행된 후, 그 뒤에 IO가 실행될 것이다. 여기서 IO는 기본적으로 5tick동안 실행되므로, IO를 먼저 선언했을떄보다 많은 시간을 소비할 것이다. 실제 결과는 다음과 같다.

```bash
PS D:\study\operatingSystem\homework\cpu-intro> python process-run.py -l 4:100,1:0 -c -p
Time    PID: 0    PID: 1       CPU       IOs
  1    RUN:cpu     READY         1
  2    RUN:cpu     READY         1
  3    RUN:cpu     READY         1
  4    RUN:cpu     READY         1
  5       DONE    RUN:io         1
  6       DONE   WAITING                   1
  7       DONE   WAITING                   1
  8       DONE   WAITING                   1
  9       DONE   WAITING                   1
 10*      DONE      DONE         1

Stats: Total Time 10
Stats: CPU Busy 6 (60.00%)
Stats: IO Busy  4 (40.00%)
```

## 5.
### 문제
> Now, run the same processes, but with the switching behavior set to switch to another process whenever one is WAITING for I/O (-l 1:0,4:100 -c -S SWITCH_ON_IO). What happens now? Use -c and -p to confirm that you are right.
> 이제, 같은 프로세스들을 실행한다. 이때 프로세스의 switch 시점을 프로세스가 끝나는 때가 아닌, IO가 끝나는 시점으로 바꾼다. (`-l 1:0,4:100 -c -S SWITCH_ON_IO` 를 입력하면 된다.) `-c`와 `-p` flag로 맞는지 확인해라
### 답안
먼저 프로세스의 switch 시점을 선행하는 프로세스가 끝나는 때로 설정하면 문제의 경우는 비효율적이다. IO 명령어 하나로 이루어진 프로세스가 먼저 실행되기 때문에 IO를 수행할때도 cpu는 사용되지 못할 것이다. 반면, IO가 수행될때 다른 프로세스를 실행할 수 있다면, cpu 사용률을 높일 수 있을 것이다. 실행 결과는 다음과 같다.
```bash
PS D:\study\operatingSystem\homework\cpu-intro> python process-run.py -l 1:0,4:100 -c -S SWITCH_ON_IO
Time    PID: 0    PID: 1       CPU       IOs
  1     RUN:io     READY         1
  2    WAITING   RUN:cpu         1         1
  3    WAITING   RUN:cpu         1         1
  4    WAITING   RUN:cpu         1         1
  5    WAITING   RUN:cpu         1         1
  6*      DONE      DONE         1
```
* IO가 수행될동안 CPU가 항상 실행되서 사용률이 100%이다.

## 7.
### 문제
> Now run the same processes, but with `-I_IO_RUN IMMEDIATE` set, which immediately runs the process that issued the I/O. How does this behavior differ? Why might running a process that just completed an I/O again be a good idea?
> 이제 같은 process들을 실행시키지만, 
### 답안
* 이 경우, 6번에 비해 더 효율적인 cpu 사용률을 기대할 수 있다. IO가 끝난 후에, IO를 요청한 process로 바로 전환되기 때문이다. IO가 3번 이루어지고, 그 3번에 따른 IO 수행시간동안 나머지 3개의 process를 실행할 수 있기 때문이다. 실제로, 이 문제에서는 100%의 cpu 사용률을 보인다.
```bash
PS D:\study\operatingSystem\homework\cpu-intro> python process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -I IO_RUN_IMMEDIATE -c -p
Time    PID: 0    PID: 1    PID: 2    PID: 3       CPU       IOs
  1     RUN:io     READY     READY     READY         1
  2    WAITING   RUN:cpu     READY     READY         1         1
  3    WAITING   RUN:cpu     READY     READY         1         1
  4    WAITING   RUN:cpu     READY     READY         1         1
  5    WAITING   RUN:cpu     READY     READY         1         1
  6*    RUN:io     READY     READY     READY         1
  7    WAITING   RUN:cpu     READY     READY         1         1
  8    WAITING      DONE   RUN:cpu     READY         1         1
  9    WAITING      DONE   RUN:cpu     READY         1         1
 10    WAITING      DONE   RUN:cpu     READY         1         1
 11*    RUN:io      DONE     READY     READY         1
 12    WAITING      DONE   RUN:cpu     READY         1         1
 13    WAITING      DONE   RUN:cpu     READY         1         1
 14    WAITING      DONE      DONE   RUN:cpu         1         1
 15    WAITING      DONE      DONE   RUN:cpu         1         1
 16*      DONE      DONE      DONE   RUN:cpu         1
 17       DONE      DONE      DONE   RUN:cpu         1
 18       DONE      DONE      DONE   RUN:cpu         1

Stats: Total Time 18
Stats: CPU Busy 18 (100.00%)
Stats: IO Busy  12 (66.67%)
```