import os
import subprocess
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm

cacheSizes = np.arange(1, 5)
policies = ["FIFO", "LRU", "OPT", "RAND", "CLOCK", "UNOPT"]
hitRates = [[50.44, 79.74, 88.68, 92.47],
[50.44, 85.85, 90.97, 94.26],
[50.44, 86.08, 93.28, 95.76],
[50.44, 79.68, 87.85, 91.71],
[50.44, 81.52, 89.48, 93.17]
]

for policy in policies[5:]:
    print(policy)
    hitRate = []
    for cacheSize in tqdm(cacheSizes):
        result = subprocess.run(['python.exe', "./paging-policy.py", "-c", "-p", policy, 
            "-f", './vpn.txt', "-C", str(cacheSize)], stdout=subprocess.PIPE)
        result = result.stdout.decode('utf-8')
        # print(result)
        # print(result[-9:-4])
        hitRate.append(float(result[-9:-4]))
    print(hitRate)
    hitRates.append(hitRate)

print(hitRates)
for i in range(len(policies)):
    plt.plot(cacheSizes, hitRates[i])

plt.legend(policies)
plt.margins(0)
plt.xticks(cacheSizes, cacheSizes)
plt.xlabel('Cache Size (Blocks)')
plt.ylabel('Hit Rate')
plt.savefig('workload.png', dpi=227)