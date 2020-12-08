# Chapter 40. File System Implementation
## 1.
### Q
Run the simulator with some different random seeds (say 17, 18, 19, 20), and see if you can figure out which operations must have taken place between each state change.
### A
수행되는 operation은 총 가지가 있다
* mkdir() - creates a new directory
* creat() - creates a new (empty) file
* open(), write(), close() - appends a block to a file
* link() - creates a hard link to a file
* unlink() - unlinks a file (removing it if linkcnt==0)
```
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 17 -c
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 18 -c
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 19 -c
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 20 -c
```
## 2.
```
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 21 -c
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 22 -c
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 23 -c
D:\study\operatingSystem\homework\file-implementation>python vsfs.py -s 24 -c
```
* 첫 block을 allocate 하는것을 선호한다.