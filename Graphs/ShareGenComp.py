"""
Share Generation for both schemes.

I think what matters the most is the overall time it takes to compute the shares which is probably
more dominated by latency and the threshold t than by anything else. We should fix a reasonable n (i.e. n=1000?)
and run the experiments with varying t and deployment configurations.

Local: Same data-center
Remote: Same continent
Distant: Different continent
"""

import matplotlib.pyplot as plt
import numpy as np

data_x = np.arange(2, 10, 1)


data_t2_s1 = [1*x**2 for x in range(3, 11)]
data_t5_s1 = [1.6*x**2 for x in range(3, 11)]
data_t9_s1 = [2*x**2 for x in range(3, 11)]

data_t2_s2 = [0.1*x**2 for x in range(3, 11)]
data_t5_s2 = [0.16*x**2 for x in range(3, 11)]
data_t9_s2 = [0.99*x**2 for x in range(3, 11)]


plt.plot(data_x, data_t2_s1, color='#1f77b4', marker='x', linestyle='-', label="Scheme 1 (Local)")
plt.plot(data_x, data_t5_s1, color='#ff7f0e', marker='+', linestyle='-', label="Scheme 1 (Remote)")
plt.plot(data_x, data_t9_s1, color='#2ca02c', marker='^', linestyle='-', label="Scheme 1 (Distant)")
plt.plot(data_x, data_t2_s2, color='#9467bd', marker='x', linestyle='dashed', label="Scheme 2 (Local)")
plt.plot(data_x, data_t5_s2, color='#8c564b', marker='+', linestyle='dashed', label="Scheme 2 (Remote)")
plt.plot(data_x, data_t9_s2, color='#e377c2', marker='^', linestyle='dashed', label="Scheme 2 (Distant)")

plt.title("Share Generation Total Overhead for m=10 Parties")
plt.ylabel("Time in Seconds")
plt.xlabel("Threshold t")
plt.grid()

plt.xticks(np.arange(data_x.min(), data_x.max()+1, 1))
plt.xlim((data_x.min(), data_x.max()))
plt.legend()

plt.savefig("sharegen_comp.pdf")
plt.show()