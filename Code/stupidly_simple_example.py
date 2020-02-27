from mod import Mod 

P = 1000000007
Q =  500000003

# P = 11
# Q =  5

K = 4

def p(x):
    a = 33
    b = 24
    c = K
    return a * x * x + b * x + c

HH = 3123

def get_share(iid):
    h = HH
    return h ** (p(iid)%(P-1)) % P

id_s=[1,2,3]

sss = [Mod(get_share(id_s[i]), P) for i in range(3)]
# print(sss)

recon = 1
for i in range(len(id_s)):
    __temp = 1
    for j in range(len(id_s)):
        if j != i:
            __temp *= id_s[i] - id_s[j]
        __temp = (1 // Mod(__temp, Q)).__int__()
    for j in range(len(id_s)):
        if j != i:
            __temp *= - id_s[j]
    print(i, __temp % Q)
    recon *= sss[i] ** (__temp % Q)

print(recon)
print(Mod(HH,P) ** K)
print(Mod(HH,P) ** (K * (Q+1)))

    



