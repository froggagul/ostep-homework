# Chapter 21. Beyond Physical Memory: Mechanisms
## 1.
`man vmstat`
> us: Time spent running non-kernel code.  (user time, including nice time)  
> id: Time spent idle.  Prior to Linux 2.5.41, this includes IO-wait time.

**실행결과**
* user time이 늘어나고 idle time이 줄어들었다.
* mem을 n개 여러개 실행시키는 경우, usertime이 n배 늘어나고, usertime이 늘어난만큼 idle time이 줄어들었다.

## 2.
**실행결과**
* `./mem 1024`를 실행시키니 free가 줄어들었다가 ^c로 프로그램을 중단시키니 free된 메모리가 원래대로 돌아왔다. swpd에는 아무런 변화가 없었다.
```
froggagul@ubuntu:~/ostep/vm-beyondphys$ vmstat 1
procs -----------memory---------- ---swap-- -----io---- -system-- ------cpu-----
 r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st
 7  0      0 4577816 251744 1770720    0    0   294   960  388  676  7  5 87  1  0
 0  0      0 4576252 251780 1771148    0    0     0  3792 2407 4426  7  6 87  1  0
 0  0      0 4576188 251804 1771292    0    0     0  3560 1178 1724  2  3 95  1  0
 1  0      0 3555676 251820 1771440    0    0     0  3572 2034 3102  8 13 78  0  0
 1  0      0 3541696 251852 1771640    0    0     0  3548 1972 2636 23 11 65  0  0
 1  0      0 3541696 251896 1771644    0    0     0  5896 1802 2858 28  4 68  0  0
 0  0      0 4592472 251912 1771640    0    0     0  3528 1249 2132 24  4 72  0  0
 3  0      0 4592324 252004 1771672    0    0     0  3732 1491 2657  2  3 95  1  0

```

## 3.
**실행결과**
* 첫 loop에는 swap in, swap out이 일어나다 이후 swap out만 일어난다. 그 이후에는 swap in과 swap out이 번갈아 일어난다.
* 첫 loop에 비해 두번째, 세번째 loop에서는 swap in, swap out이 되는 메모리 크기가 점점 줄어든다.