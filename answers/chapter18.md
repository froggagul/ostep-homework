# Chapter 18. Paging: Introduction
## 1.
Before doing any translations, let’s use the simulator to study how linear page tables change size given different parameters. Compute the size of linear page tables as different parameters change. Some suggested inputs are below; by using the `-v` flag,you can see how many page-table entries are filled. First, to understand how linear page table size changes as the address space grows:
```
./paging-linear-translate.py -P 1k -a 1m -p 512m -v -n 0
./paging-linear-translate.py -P 1k -a 2m -p 512m -v -n 0
./paging-linear-translate.py -P 1k -a 4m -p 512m -v -n 0
```
Then, to understand how linear page table size changes as page size grows:
```
./paging-linear-translate.py -P 1k -a 1m -p 512m -v -n 0
./paging-linear-translate.py -P 2k -a 1m -p 512m -v -n 0
./paging-linear-translate.py -P 4k -a 1m -p 512m -v -n 0
```
Before running any of these, try to think about the expected trends. How should page-table size change as the address space grows? As thepage size grows? Why shouldn’t we just use really big pages in general?
### Answer
page-table size는 address space / page size 이므로, address space가 커지면서 page-table size도 비례하며 커진다. 너무 큰 page는 낭비가 심하기에 이를 적당히 제어해야한다.

## 2. 
Now let’s do some translations. Start with some small examples, and change the number of pages that are allocated to the address space with the `-u` flag. For example:
```
./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 0
./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 25
./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 50
./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 75
./paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 100
```
What happens as you increase the percentage of pages that are allocated in each address space?
### Answer
* 처음에는 아무런 address space를 사용하지 않아 모든 memory access가 실패했다.
* 그러나 사용률이 늘어나면서 더 많은 page들이 valid해진다.

## 3.
Now let’s try some different random seeds, and some different (and sometimes quite crazy) address-space parameters, for variety:
```
./paging-linear-translate.py -P 8  -a 32   -p 1024 -v -s 1
./paging-linear-translate.py -P 8k -a 32k  -p 1m   -v -s 2
./paging-linear-translate.py -P 1m -a 256m -p 512m -v -s 3
```
Which of these parameter combinations are unrealistic? Why?
### Answer
첫번쨰 것은 너무 작아 unrealistic하다.
