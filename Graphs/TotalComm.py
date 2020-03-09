"""
Total Communication Costs for both Schemes:

Includes: (1) Share generation
(2) Sending shares to parties
(3) Opening values in t-intersection

"""

import matplotlib.pyplot as plt

data_x = [2**x for x in range(3, 16)]


data_t2_s1 = [1*x**2 for x in range(3, 16)]
data_t5_s1 = [1.6*x**2 for x in range(3, 16)]
data_t9_s1 = [2*x**2 for x in range(3, 16)]

data_t2_s2 = [0.1*x**2 for x in range(3, 16)]
data_t5_s2 = [0.16*x**2 for x in range(3, 16)]
data_t9_s2 = [0.99*x**2 for x in range(3, 16)]


plt.plot(data_x, data_t2_s1, color='#1f77b4', marker='x', linestyle='-', label="Scheme 1 (m=10, t=2)")
plt.plot(data_x, data_t5_s1, color='#ff7f0e', marker='+', linestyle='-', label="Scheme 1 (m=10, t=5)")
plt.plot(data_x, data_t9_s1, color='#2ca02c', marker='^', linestyle='-', label="Scheme 1 (m=10, t=9)")
plt.plot(data_x, data_t2_s2, color='#9467bd', marker='x', linestyle='dashed', label="Scheme 2 (m=10, t=2)")
plt.plot(data_x, data_t5_s2, color='#8c564b', marker='+', linestyle='dashed', label="Scheme 2 (m=10, t=5)")
plt.plot(data_x, data_t9_s2, color='#e377c2', marker='^', linestyle='dashed', label="Scheme 2 (m=10, t=9)")

plt.title("Total Communication")
plt.ylabel("Communication in MB")
plt.xlabel("Number of Elements per Party")
plt.grid()

plt.xscale("log", basex=2)
plt.xlim((2**3, 2**16))
plt.legend()

plt.savefig("total_comm.pdf")
plt.show()