# Chapter 27. Interlude: Thread API
사용 코드: threads-api 안의 코드들
## 1.
### 문제
> First build main-race.c. Examine the code so you can see the (hopefully obvious) data race in the code. Now run helgrind (by typing valgrind --tool=helgrind main-race) to see how it reports the race. Does it point to the right lines of code? What other information does it give to you?

> 먼저 main-race.c를 빌드하세요. code를 관찰해 data race가 존재하는지 확인하세요. 그리고 helgrind를 실행시켜 어떻게 race를 보고하는지 관찰하세요. code의 정확한 line을 기리키나요? 다른 정보로는 무엇을 주나요?

### 답안
정확한 line을 가리키며, data race가 발생할 수 있는 thread의 번호, 충돌하는 주소, 정보와 그 크기를 준다.
```
froggagul@ubuntu:~/ostep/threads-api$ valgrind --tool=helgrind ./main-race
(중략)
==4830== Possible data race during read of size 4 at 0x10C014 by thread #1
==4830== Locks held: none
==4830==    at 0x10922D: main (main-race.c:15)
==4830== 
==4830== This conflicts with a previous write of size 4 by thread #2
==4830== Locks held: none
==4830==    at 0x1091BE: worker (main-race.c:8)
==4830==    by 0x4842B1A: ??? (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_helgrind-amd64-linux.so)
==4830==    by 0x486A608: start_thread (pthread_create.c:477)
==4830==    by 0x49A6102: clone (clone.S:95)
==4830==  Address 0x10c014 is 0 bytes inside data symbol "balance"
==4830== 
==4830== ----------------------------------------------------------------
==4830== 
==4830== Possible data race during write of size 4 at 0x10C014 by thread #1
==4830== Locks held: none
==4830==    at 0x109236: main (main-race.c:15)
==4830== 
==4830== This conflicts with a previous write of size 4 by thread #2
==4830== Locks held: none
==4830==    at 0x1091BE: worker (main-race.c:8)
==4830==    by 0x4842B1A: ??? (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_helgrind-amd64-linux.so)
==4830==    by 0x486A608: start_thread (pthread_create.c:477)
==4830==    by 0x49A6102: clone (clone.S:95)
==4830==  Address 0x10c014 is 0 bytes inside data symbol "balance"
(중략)
==4830== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```
## 2.
### 문제
What happens when you remove one of the offending lines of code? Now add a lock around one of the updates to the shared variable, and then around both. What does helgrind report in each of these cases?
### 답안
```
(중략)
==4997== Use --history-level=approx or =none to gain increased speed, at
==4997== the cost of reduced accuracy of conflicting-access information
==4997== For lists of detected and suppressed errors, rerun with: -s
==4997== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
* 가능한 data race가 존재하지 않는다.

## 3.
### 문제
Now let’s look at main-deadlock.c. Examine the code. This code has a problem known as deadlock (which we discuss in much more depth in a forthcoming chapter). Can you see what problem it might have?
### 답안
* 각 thread가 서로를 lock하는 상황이 생긴다.