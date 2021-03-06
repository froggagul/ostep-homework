# Chapter 9. Scheduling: Proportional Share
사용 코드는 다음과 같다
* cpu-sched-multi/multi.py
## 1.
### 질문
To start things off, let’s learn how to use the simulator to study how to build an effective multi-processor scheduler. The first simulation will run just one job, which has a run-time of 30, and a working-set size of 200. Run this job (called job ’a’ here) on one simulated CPU as follows: ./multi.py -n 1 -L a:30:200. How long will it take to complete? Turn on the -c flag to see a final answer, and the -t flag to see a tick-by-tick trace of the job and how it is scheduled.
### 답안
cache의 default size가 100이기 때문에, cache가 a를 실행하는데 도움주지 못한다(cold한 상태). 따라서, Runtime이 30일 것이다.
```shell
PS D:\study\operatingSystem\homework\cpu-sched-multi> python multi.py -n 1 -L a:30:101 -c -t
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:30:101
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 1
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 100
ARG random_order False
ARG trace True
ARG trace_time False
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:30 working_set_size:101

Scheduler central queue: ['a']

   0   a
   1   a
   2   a
   3   a
(중략)
  26   a
  27   a
  28   a
  29   a

Finished time 30

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 0.00 ]
```

## 2.
### 질문
Now increase the cache size so as to make the job’s working set (size=200) fit into the cache (which, by default, is size=100); for example, run ./multi.py -n 1 -L a:30:200 -M 300. Can you predict how fast the job will run once it fits in cache? (hint: remember the key parameter of the warm rate, which is set by the -r flag) Check your answer by running with the solve flag (-c) enabled.
### 답안
1번과 달리 cache가 10 unit time인 warn-up time 이후에 프로그램의 실행을 2배 빨라지게 하기 때문에, 실행 시간은 10 + 20 / 2 = 20(unit time)이다.
```shell
PS D:\study\operatingSystem\homework\cpu-sched-multi> python multi.py -n 1 -L a:30:200 -M 300 -c -t
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:30:200
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 1
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 300
ARG random_order False
ARG trace True
ARG trace_time False
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:30 working_set_size:200

Scheduler central queue: ['a']

   0   a
   1   a
   2   a
   3   a
   4   a
(중략)
  15   a
  16   a
  17   a
  18   a
  19   a

Finished time 20

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 50.00 ]
```
## 3.
### 질문
One cool thing about multi.py is that you can see more detail about what is going on with different tracing flags. Run the same simulation as above, but this time with time left tracing enabled (-T). This flag shows both the job that was scheduled on a CPU at each time step, as well as how much run-time that job has left after each tick has run. What do you notice about how that second column decreases?
### 답안
```
PS D:\study\operatingSystem\homework\cpu-sched-multi> python multi.py -n 1 -L a:30:200 -M 300 -c -t -T
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:30:200
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 1
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 300
ARG random_order False
ARG trace True
ARG trace_time True
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:30 working_set_size:200

Scheduler central queue: ['a']

   0   a [ 29]
   1   a [ 28]
   2   a [ 27]
   3   a [ 26]
   4   a [ 25]
   5   a [ 24]
   6   a [ 23]
   7   a [ 22]
   8   a [ 21]
   9   a [ 20]
----------------
  10   a [ 18]
  11   a [ 16]
  12   a [ 14]
  13   a [ 12]
  14   a [ 10]
  15   a [  8]
  16   a [  6]
  17   a [  4]
  18   a [  2]
  19   a [  0]

Finished time 20

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 50.00 ]
```
처음 10 unit time동안은 run-time이 1씩 줄어들지만, cache가 warm up이 되고난 10 unit time 이후에는 2씩 줄어든다.

## 4.
### 질문
Now add one more bit of tracing, to show the status of each CPU cache for each job, with the -C flag. For each job, each cache will either show a blank space (if the cache is cold for that job) or a ’w’ (if the cache is warm for that job). At what point does the cache become warm for job ’a’ in this simple example? What happens as you change the warmup time parameter (-w) to lower or higher values than the default?
### 답안
warm up 시간이 늦어지면 그만큼 1 씩 줄어드는 시간이 늘어나므로, process가 끝나는 시간이 증가한다. 빨라지는 경우는 그 반대이다.

## 5.
### 질문
At this point, you should have a good idea of how the simulator works for a single job running on a single CPU. But hey, isn’t this a multi-processor CPU scheduling chapter? Oh yeah! So let’s start working with multiple jobs. Specifically, let’s run the following three jobs on a two-CPU system (i.e., type ./multi.py -n 2 -L a:100:100,b:100:50,c:100:50) Can you predict how long this will take, given a round-robin centralized scheduler? Use -c to see if you were right, and then dive down into details with -t to see a step-by-step and then -C to see whether caches got warmed effectively for these jobs. What do you notice?
### 답안
round-robin centralized scheduler의 경우, 실행 순서가 첫번째 cpu는  a->c->b, 두번째 cpu는 b->a->c이다. 이중 어느경우도 warm-up된 cache를 사용하지 못한다. 같은 process가 돌아오기까지 20unit time이 소요되는데, cache에 머무를 수 있는 최대 시간이 20unit time이다(working-set-size가 50인 process가 연속해서 오는경우). 따라서, 두개의 cpu에서 cache 없이 실행되는 것과 같은효과를 내어 200 unit time의 시간이 a, b, c를 행하는데 소요된다.

```shell
PS D:\study\operatingSystem\homework\cpu-sched-multi> python ./multi.py -n 2 -L a:100:100,b:100:50,c:100:50 -c -t -T -C
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:100:100,b:100:50,c:100:50
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 2
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 100
ARG random_order False
ARG trace True
ARG trace_time True
ARG trace_cache True
ARG trace_sched False
ARG compute True

Job name:a run_time:100 working_set_size:100
Job name:b run_time:100 working_set_size:50
Job name:c run_time:100 working_set_size:50

Scheduler central queue: ['a', 'b', 'c']

   0   a [ 99] cache[   ]     b [ 99] cache[   ]
   1   a [ 98] cache[   ]     b [ 98] cache[   ]
   2   a [ 97] cache[   ]     b [ 97] cache[   ]
   3   a [ 96] cache[   ]     b [ 96] cache[   ]
   4   a [ 95] cache[   ]     b [ 95] cache[   ]
   5   a [ 94] cache[   ]     b [ 94] cache[   ]
   6   a [ 93] cache[   ]     b [ 93] cache[   ]
   7   a [ 92] cache[   ]     b [ 92] cache[   ]
   8   a [ 91] cache[   ]     b [ 91] cache[   ]
   9   a [ 90] cache[w  ]     b [ 90] cache[ w ]
---------------------------------------------------
  10   c [ 99] cache[w  ]     a [ 89] cache[ w ]
  11   c [ 98] cache[w  ]     a [ 88] cache[ w ]
  12   c [ 97] cache[w  ]     a [ 87] cache[ w ]
  13   c [ 96] cache[w  ]     a [ 86] cache[ w ]
  14   c [ 95] cache[w  ]     a [ 85] cache[ w ]
  15   c [ 94] cache[w  ]     a [ 84] cache[ w ]
  16   c [ 93] cache[w  ]     a [ 83] cache[ w ]
  17   c [ 92] cache[w  ]     a [ 82] cache[ w ]
  18   c [ 91] cache[w  ]     a [ 81] cache[ w ]
  19   c [ 90] cache[  w]     a [ 80] cache[w  ]
---------------------------------------------------
  20   b [ 89] cache[  w]     c [ 89] cache[w  ]
  21   b [ 88] cache[  w]     c [ 88] cache[w  ]
  22   b [ 87] cache[  w]     c [ 87] cache[w  ]
  23   b [ 86] cache[  w]     c [ 86] cache[w  ]
  24   b [ 85] cache[  w]     c [ 85] cache[w  ]
  25   b [ 84] cache[  w]     c [ 84] cache[w  ]
  26   b [ 83] cache[  w]     c [ 83] cache[w  ]
  27   b [ 82] cache[  w]     c [ 82] cache[w  ]
  28   b [ 81] cache[  w]     c [ 81] cache[w  ]
  29   b [ 80] cache[ ww]     c [ 80] cache[  w]
---------------------------------------------------
  30   a [ 79] cache[ ww]     b [ 79] cache[  w]
  31   a [ 78] cache[ ww]     b [ 78] cache[  w]
  32   a [ 77] cache[ ww]     b [ 77] cache[  w]
  33   a [ 76] cache[ ww]     b [ 76] cache[  w]
  34   a [ 75] cache[ ww]     b [ 75] cache[  w]
  35   a [ 74] cache[ ww]     b [ 74] cache[  w]
  36   a [ 73] cache[ ww]     b [ 73] cache[  w]
  37   a [ 72] cache[ ww]     b [ 72] cache[  w]
  38   a [ 71] cache[ ww]     b [ 71] cache[  w]
  39   a [ 70] cache[w  ]     b [ 70] cache[ ww]
---------------------------------------------------
  40   c [ 79] cache[w  ]     a [ 69] cache[ ww]
  41   c [ 78] cache[w  ]     a [ 68] cache[ ww]
  42   c [ 77] cache[w  ]     a [ 67] cache[ ww]
  43   c [ 76] cache[w  ]     a [ 66] cache[ ww]
  44   c [ 75] cache[w  ]     a [ 65] cache[ ww]
  45   c [ 74] cache[w  ]     a [ 64] cache[ ww]
  46   c [ 73] cache[w  ]     a [ 63] cache[ ww]
  47   c [ 72] cache[w  ]     a [ 62] cache[ ww]
  48   c [ 71] cache[w  ]     a [ 61] cache[ ww]
  49   c [ 70] cache[  w]     a [ 60] cache[w  ]
---------------------------------------------------
  50   b [ 69] cache[  w]     c [ 69] cache[w  ]
  51   b [ 68] cache[  w]     c [ 68] cache[w  ]
  52   b [ 67] cache[  w]     c [ 67] cache[w  ]
  53   b [ 66] cache[  w]     c [ 66] cache[w  ]
  54   b [ 65] cache[  w]     c [ 65] cache[w  ]
  55   b [ 64] cache[  w]     c [ 64] cache[w  ]
  56   b [ 63] cache[  w]     c [ 63] cache[w  ]
  57   b [ 62] cache[  w]     c [ 62] cache[w  ]
  58   b [ 61] cache[  w]     c [ 61] cache[w  ]
  59   b [ 60] cache[ ww]     c [ 60] cache[  w]
---------------------------------------------------
  60   a [ 59] cache[ ww]     b [ 59] cache[  w]
  61   a [ 58] cache[ ww]     b [ 58] cache[  w]
  62   a [ 57] cache[ ww]     b [ 57] cache[  w]
  63   a [ 56] cache[ ww]     b [ 56] cache[  w]
  64   a [ 55] cache[ ww]     b [ 55] cache[  w]
  65   a [ 54] cache[ ww]     b [ 54] cache[  w]
  66   a [ 53] cache[ ww]     b [ 53] cache[  w]
  67   a [ 52] cache[ ww]     b [ 52] cache[  w]
  68   a [ 51] cache[ ww]     b [ 51] cache[  w]
  69   a [ 50] cache[w  ]     b [ 50] cache[ ww]
---------------------------------------------------
  70   c [ 59] cache[w  ]     a [ 49] cache[ ww]
  71   c [ 58] cache[w  ]     a [ 48] cache[ ww]
  72   c [ 57] cache[w  ]     a [ 47] cache[ ww]
  73   c [ 56] cache[w  ]     a [ 46] cache[ ww]
  74   c [ 55] cache[w  ]     a [ 45] cache[ ww]
  75   c [ 54] cache[w  ]     a [ 44] cache[ ww]
  76   c [ 53] cache[w  ]     a [ 43] cache[ ww]
  77   c [ 52] cache[w  ]     a [ 42] cache[ ww]
  78   c [ 51] cache[w  ]     a [ 41] cache[ ww]
  79   c [ 50] cache[  w]     a [ 40] cache[w  ]
---------------------------------------------------
  80   b [ 49] cache[  w]     c [ 49] cache[w  ]
  81   b [ 48] cache[  w]     c [ 48] cache[w  ]
  82   b [ 47] cache[  w]     c [ 47] cache[w  ]
  83   b [ 46] cache[  w]     c [ 46] cache[w  ]
  84   b [ 45] cache[  w]     c [ 45] cache[w  ]
  85   b [ 44] cache[  w]     c [ 44] cache[w  ]
  86   b [ 43] cache[  w]     c [ 43] cache[w  ]
  87   b [ 42] cache[  w]     c [ 42] cache[w  ]
  88   b [ 41] cache[  w]     c [ 41] cache[w  ]
  89   b [ 40] cache[ ww]     c [ 40] cache[  w]
---------------------------------------------------
  90   a [ 39] cache[ ww]     b [ 39] cache[  w]
  91   a [ 38] cache[ ww]     b [ 38] cache[  w]
  92   a [ 37] cache[ ww]     b [ 37] cache[  w]
  93   a [ 36] cache[ ww]     b [ 36] cache[  w]
  94   a [ 35] cache[ ww]     b [ 35] cache[  w]
  95   a [ 34] cache[ ww]     b [ 34] cache[  w]
  96   a [ 33] cache[ ww]     b [ 33] cache[  w]
  97   a [ 32] cache[ ww]     b [ 32] cache[  w]
  98   a [ 31] cache[ ww]     b [ 31] cache[  w]
  99   a [ 30] cache[w  ]     b [ 30] cache[ ww]
---------------------------------------------------
 100   c [ 39] cache[w  ]     a [ 29] cache[ ww]
 101   c [ 38] cache[w  ]     a [ 28] cache[ ww]
 102   c [ 37] cache[w  ]     a [ 27] cache[ ww]
 103   c [ 36] cache[w  ]     a [ 26] cache[ ww]
 104   c [ 35] cache[w  ]     a [ 25] cache[ ww]
 105   c [ 34] cache[w  ]     a [ 24] cache[ ww]
 106   c [ 33] cache[w  ]     a [ 23] cache[ ww]
 107   c [ 32] cache[w  ]     a [ 22] cache[ ww]
 108   c [ 31] cache[w  ]     a [ 21] cache[ ww]
 109   c [ 30] cache[  w]     a [ 20] cache[w  ]
---------------------------------------------------
 110   b [ 29] cache[  w]     c [ 29] cache[w  ]
 111   b [ 28] cache[  w]     c [ 28] cache[w  ]
 112   b [ 27] cache[  w]     c [ 27] cache[w  ]
 113   b [ 26] cache[  w]     c [ 26] cache[w  ]
 114   b [ 25] cache[  w]     c [ 25] cache[w  ]
 115   b [ 24] cache[  w]     c [ 24] cache[w  ]
 116   b [ 23] cache[  w]     c [ 23] cache[w  ]
 117   b [ 22] cache[  w]     c [ 22] cache[w  ]
 118   b [ 21] cache[  w]     c [ 21] cache[w  ]
 119   b [ 20] cache[ ww]     c [ 20] cache[  w]
---------------------------------------------------
 120   a [ 19] cache[ ww]     b [ 19] cache[  w]
 121   a [ 18] cache[ ww]     b [ 18] cache[  w]
 122   a [ 17] cache[ ww]     b [ 17] cache[  w]
 123   a [ 16] cache[ ww]     b [ 16] cache[  w]
 124   a [ 15] cache[ ww]     b [ 15] cache[  w]
 125   a [ 14] cache[ ww]     b [ 14] cache[  w]
 126   a [ 13] cache[ ww]     b [ 13] cache[  w]
 127   a [ 12] cache[ ww]     b [ 12] cache[  w]
 128   a [ 11] cache[ ww]     b [ 11] cache[  w]
 129   a [ 10] cache[w  ]     b [ 10] cache[ ww]
---------------------------------------------------
 130   c [ 19] cache[w  ]     a [  9] cache[ ww]
 131   c [ 18] cache[w  ]     a [  8] cache[ ww]
 132   c [ 17] cache[w  ]     a [  7] cache[ ww]
 133   c [ 16] cache[w  ]     a [  6] cache[ ww]
 134   c [ 15] cache[w  ]     a [  5] cache[ ww]
 135   c [ 14] cache[w  ]     a [  4] cache[ ww]
 136   c [ 13] cache[w  ]     a [  3] cache[ ww]
 137   c [ 12] cache[w  ]     a [  2] cache[ ww]
 138   c [ 11] cache[w  ]     a [  1] cache[ ww]
 139   c [ 10] cache[  w]     a [  0] cache[w  ]
---------------------------------------------------
 140   b [  9] cache[  w]     c [  9] cache[w  ]
 141   b [  8] cache[  w]     c [  8] cache[w  ]
 142   b [  7] cache[  w]     c [  7] cache[w  ]
 143   b [  6] cache[  w]     c [  6] cache[w  ]
 144   b [  5] cache[  w]     c [  5] cache[w  ]
 145   b [  4] cache[  w]     c [  4] cache[w  ]
 146   b [  3] cache[  w]     c [  3] cache[w  ]
 147   b [  2] cache[  w]     c [  2] cache[w  ]
 148   b [  1] cache[  w]     c [  1] cache[w  ]
 149   b [  0] cache[ ww]     c [  0] cache[  w]

Finished time 150

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 0.00 ]
  CPU 1  utilization 100.00 [ warm 0.00 ]
```