# Chapter 41. Crash Consistency: FSCK and Journaling
## 1.
```
D:\study\operatingSystem\homework\file-lfs>python lfs.py -n 3 -o -c -i

INITIAL file system contents:
[   0 ] live checkpoint: 3 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
[   1 ] live [.,0] [..,0] -- -- -- -- -- --
[   2 ] live type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- --
[   3 ] live chunk(imap): 2 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 

create file /ku3

[   0 ] live checkpoint: 7 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[   4 ] live [.,0] [..,0] [ku3,1] -- -- -- -- --
[   5 ] live type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
[   6 ] live type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[   7 ] live chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --


write file  /ku3 offset=7 size=4

[   0 ] live checkpoint: 10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[   8 ] live z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0
[   9 ] live type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 8
[  10 ] live chunk(imap): 5 9 -- -- -- -- -- -- -- -- -- -- -- -- -- --


create file /qg9

[   0 ] live checkpoint: 14 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[  11 ] live [.,0] [..,0] [ku3,1] [qg9,2] -- -- -- --
[  12 ] live type:dir size:1 refs:2 ptrs: 11 -- -- -- -- -- -- --
[  13 ] live type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[  14 ] live chunk(imap): 12 9 13 -- -- -- -- -- -- -- -- -- -- -- -- --



FINAL file system contents:
[   0 ] live checkpoint: 14 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
[   1 ]      [.,0] [..,0] -- -- -- -- -- --
[   2 ]      type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- --
[   3 ]      chunk(imap): 2 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
[   4 ]      [.,0] [..,0] [ku3,1] -- -- -- -- --
[   5 ]      type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
[   6 ]      type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[   7 ]      chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --
[   8 ] live z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0z0
[   9 ] live type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 8
[  10 ]      chunk(imap): 5 9 -- -- -- -- -- -- -- -- -- -- -- -- -- --
[  11 ] live [.,0] [..,0] [ku3,1] [qg9,2] -- -- -- --
[  12 ] live type:dir size:1 refs:2 ptrs: 11 -- -- -- -- -- -- --
[  13 ] live type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[  14 ] live chunk(imap): 12 9 13 -- -- -- -- -- -- -- -- -- -- -- -- --
```
## 3.
```
create file /us7

[   0 ] live checkpoint: 7 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[   4 ] live [.,0] [..,0] [us7,1] -- -- -- -- --
[   5 ] live type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
[   6 ] live type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[   7 ] live chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --


write file  /us7 offset=4 size=0

[   0 ] live checkpoint: 9 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[   8 ] live type:reg size:4 refs:1 ptrs: -- -- -- -- -- -- -- --
[   9 ] live chunk(imap): 5 8 -- -- -- -- -- -- -- -- -- -- -- -- -- --


write file  /us7 offset=7 size=7

[   0 ] live checkpoint: 12 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[  10 ] live i0i0i0i0i0i0i0i0i0i0i0i0i0i0i0i0
[  11 ] live type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 10
[  12 ] live chunk(imap): 5 11 -- -- -- -- -- -- -- -- -- -- -- -- -- --
```
## 5.
```
python ./lfs.py -L c,/foo:w,/foo,0,1:w,/foo,1,1:w,/foo,2,1:w,/foo,3,1 -o -c
create file /foo

[   0 ] live checkpoint: 7 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[   4 ] live [.,0] [..,0] [foo,1] -- -- -- -- --
[   5 ] live type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
[   6 ] live type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[   7 ] live chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --


write file  /foo offset=0 size=1

[   0 ] live checkpoint: 10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[   8 ] live v0v0v0v0v0v0v0v0v0v0v0v0v0v0v0v0
[   9 ] live type:reg size:1 refs:1 ptrs: 8 -- -- -- -- -- -- -- 
[  10 ] live chunk(imap): 5 9 -- -- -- -- -- -- -- -- -- -- -- -- -- --


write file  /foo offset=1 size=1

[   0 ] live checkpoint: 13 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[  11 ] live t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0
[  12 ] live type:reg size:2 refs:1 ptrs: 8 11 -- -- -- -- -- --
[  13 ] live chunk(imap): 5 12 -- -- -- -- -- -- -- -- -- -- -- -- -- --


write file  /foo offset=2 size=1

[   0 ] live checkpoint: 16 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[  14 ] live k0k0k0k0k0k0k0k0k0k0k0k0k0k0k0k0
[  15 ] live type:reg size:3 refs:1 ptrs: 8 11 14 -- -- -- -- --
[  16 ] live chunk(imap): 5 15 -- -- -- -- -- -- -- -- -- -- -- -- -- -- 


write file  /foo offset=3 size=1

[   0 ] live checkpoint: 19 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
[  17 ] live g0g0g0g0g0g0g0g0g0g0g0g0g0g0g0g0
[  18 ] live type:reg size:4 refs:1 ptrs: 8 11 14 17 -- -- -- --
[  19 ] live chunk(imap): 5 18 -- -- -- -- -- -- -- -- -- -- -- -- -- --



FINAL file system contents:
[   0 ] live checkpoint: 19 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
[   1 ]      [.,0] [..,0] -- -- -- -- -- --
[   2 ]      type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- --
[   3 ]      chunk(imap): 2 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
[   4 ] live [.,0] [..,0] [foo,1] -- -- -- -- --
[   5 ] live type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- --
[   6 ]      type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
[   7 ]      chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
[   8 ] live v0v0v0v0v0v0v0v0v0v0v0v0v0v0v0v0
[   9 ]      type:reg size:1 refs:1 ptrs: 8 -- -- -- -- -- -- --
[  10 ]      chunk(imap): 5 9 -- -- -- -- -- -- -- -- -- -- -- -- -- --
[  11 ] live t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0t0
[  12 ]      type:reg size:2 refs:1 ptrs: 8 11 -- -- -- -- -- --
[  13 ]      chunk(imap): 5 12 -- -- -- -- -- -- -- -- -- -- -- -- -- --
[  14 ] live k0k0k0k0k0k0k0k0k0k0k0k0k0k0k0k0
[  15 ]      type:reg size:3 refs:1 ptrs: 8 11 14 -- -- -- -- --
[  16 ]      chunk(imap): 5 15 -- -- -- -- -- -- -- -- -- -- -- -- -- --
[  17 ] live g0g0g0g0g0g0g0g0g0g0g0g0g0g0g0g0
[  18 ] live type:reg size:4 refs:1 ptrs: 8 11 14 17 -- -- -- --
[  19 ] live chunk(imap): 5 18 -- -- -- -- -- -- -- -- -- -- -- -- -- --
```
## 7.
* 첫번째 instruction: 
* 두번째 instruction: Write one block at the end.
* 첫번쨰 instruction은 size가 1이고 두번쨰 instruction은 size가 8이다.
```
live type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- --
```
```
live type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 8
```