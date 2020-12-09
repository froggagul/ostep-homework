# Chapter 42. Crash Consistency: FSCK and Journaling
## 1.
```

D:\study\operatingSystem\homework\file-journaling>python fsck.py -D -p
ARG seed 0
ARG seedCorrupt 0
ARG numInodes 16
ARG numData 16
ARG numRequests 15
ARG printFinal True
ARG whichCorrupt -1
ARG dontCorrupt True

Final state of file system:

inode bitmap 1000100010000101
inodes       [d a:0 r:4] [] [] [] [d a:12 r:2] [] [] [] [d a:6 r:2] [] [] [] [] [f a:-1 r:2] [] [f a:-1 r:1]
data bitmap  1000001000001000
data         [(.,0) (..,0) (g,8) (w,4) (m,13) (z,13)] [] [] [] [] [] [(.,8) (..,0) (s,15)] [] [] [] [] [] [(.,4) (..,0)] [] [] []

Can you figure out which files and directories exist?


Summary of files, directories::
  Files:       ['/m', '/z', '/g/s']
  Directories: ['/', '/g', '/w']
```
* datablock에 주어진 inode 정보, inode에 있는 d/f 구분을 이용해 어느 file과 directory가 존재하는지 추론했다.
## 2.
```
D:\study\operatingSystem\homework\file-journaling>python fsck.py -S 1 -c
ARG seed 0
ARG seedCorrupt 1
ARG numInodes 16
ARG numData 16
ARG numRequests 15
ARG printFinal False
ARG whichCorrupt -1
ARG dontCorrupt False

Initial state of file system:

inode bitmap 1000100010000101
inodes       [d a:0 r:4] [] [] [] [d a:12 r:2] [] [] [] [d a:6 r:2] [] [] [] [] [f a:-1 r:2] [] [f a:-1 r:1]
data bitmap  1000001000001000
data         [(.,0) (..,0) (g,8) (w,4) (m,13) (z,13)] [] [] [] [] [] [(.,8) (..,0) (s,15)] [] [] [] [] [] [(.,4) (..,0)] [] [] []

CORRUPTION::INODE BITMAP corrupt bit 13

Final state of file system:

inode bitmap 1000100010000001
inodes       [d a:0 r:4] [] [] [] [d a:12 r:2] [] [] [] [d a:6 r:2] [] [] [] [] [f a:-1 r:2] [] [f a:-1 r:1]
data bitmap  1000001000001000
data         [(.,0) (..,0) (g,8) (w,4) (m,13) (z,13)] [] [] [] [] [] [(.,8) (..,0) (s,15)] [] [] [] [] [] [(.,4) (..,0)] [] [] []

```
* inode bitmap의 13번쨰 bit가 corruption이 일어났다.
## 3.
`fsck.py -S 3`: 15번쨰 inode를 지니는 /g/s가 1개의 reference를 지니고 있다 하지만, 실제로는 2개의 reference를 지니고 있다.
`fsck.py -S 19`: 8번째 inode를 지니는 /w가 2개의 reference를 지니고 있다 하지만, 실제로는 1개의 reference를 지니고 있다.