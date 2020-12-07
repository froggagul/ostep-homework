# Chapter 38. Redundant Arrays of Inexpensive Disks (RAIDs)
## 1.
raid 0, 1, 4의 disk 구조를 관찰했다.
```
python ./raid.py -L 0 -c -W seq
python ./raid.py -L 1 -c -W seq
python ./raid.py -L 4 -c -W seq
```
결과는 다음과 같다.
```
raid 0
0 1 2 3
4 5 6 7
8 9

raid 1
0 0 1 1
2 2 3 3
4 4 5 5
6 6 7 7
8 8 9 9

raid 4
0 1 2 P
4 5 P 3
8 P 6 7
```
raid 5의 경우, Left-symmetric/left-asymmetric일때 disk 구조를 관찰했다.
```
python ./raid.py -L 5 -5 LS -c -W seq
python ./raid.py -L 5 -5 LA -c -W seq
```
결과는 다음과 같다. P는 parity beat를 나타낸다.
```
left-symmetric    left-asymmetric
0 1 2 P           0 1 2 P
4 5 P 3           3 4 P 5
8 P 6 7           6 P 7 8
```
## 2.
chunk size를 주어지는 load의 두배인 8K로 하여 디스크 구성을 관찰했다.
```
python ./raid.py -L 5 -5 LS -c -W seq -C 8K -n 12
```
디스크 구성은 다음과 같다.
```
0  2  4  P
1  3  5  P
8 10  P  6
9 11  P  7
```
* 하나의 chunk에 두개의 load가 들어간다.
## 5.
request가 100개인 경우, total time을 측정하였다.
```
python ./raid.py -L 0 -t -n 100 -c
python ./raid.py -L 1 -t -n 100 -c
python ./raid.py -L 4 -t -n 100 -c
python ./raid.py -L 5 -t -n 100 -c
```
각 결과는 다음과 같다.
* raid0: 275.7
* raid1: 278.7
* raid4: 386.1
* raid5: 276.5

## 6.
disk의 수를 두배 늘려 8개로 하였다.
```
python ./raid.py -L 0 -t -n 100 -c -D 8
python ./raid.py -L 1 -t -n 100 -c -D 8
python ./raid.py -L 4 -t -n 100 -c -D 8
python ./raid.py -L 5 -t -n 100 -c -D 8
```
* raid0: 275.7 -> 156.5 (1.76 배)
* raid1: 278.7 -> 167.8 (1.66 배)
* raid4: 386.1 -> 165.0 (2.34 배)
* raid5: 276.5 -> 158.6 (1.74 배)