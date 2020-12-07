import random
import numpy as np
import os

np.random.seed(0)

addrList = list(np.random.randint(10, size=(10)))
addrList = ','.join(map(str, addrList)) 

print(f'addrList: {addrList}')

policies = ['LRU', 'RAND', 'CLOCK -b 0', 'CLOCK -b 1', 'CLOCK -b 3']
for policy in policies:
    cmd = f'python ./paging-policy.py --addresses={addrList} --policy={policy} -c'
    print(cmd)
    os.system(cmd)