import json
from mod import Mod

p = 1000000007
q =  500000003


with open('prototype/test_cases/ss2_match.json') as file:
    
    shares = json.load(file)
    id_s_q = []
    id_s_2q = []
    for share in shares:
        id_s_q.append(Mod(share["id"], q))
        id_s_2q.append(Mod(share["id"], p-1))
        # print(id_s_q)
    
    sec = Mod(1, p)
    mac = Mod(1, p)
    for i, share in enumerate(shares):
        __temp = Mod(1, q)
        for j in range(len(shares)):
            if j != i:
                __temp *=  id_s_q[i] - id_s_q[j]
                # print('a:', (id_s_q[j] - id_s_q[i]))
                # print('b', 1 //(id_s_q[j] - id_s_q[i]))
        __temp = 1 // __temp
        __temp = Mod(int(__temp), p-1)
        for j in range(len(shares)):
            if j != i:
                __temp *= - id_s_2q[j]
            # print(__temp)
        # print(int(__temp))
        # print()
        sec *= Mod(int(share['SS']), p) ** int(__temp)
        mac *= Mod(int(share['SS_MAC']), p) ** int(__temp)

    # sec = sec ** 2
    # mac = mac ** 2

    print(sec)
    print(mac)
    print(sec ** 5)
    print(sec ** (5 * (q+1)))


    

# with open('prototype/test_cases/ss1_match.json') as file:
    
#     shares = json.load(file)
#     id_s_q = []
#     for share in shares:
#         id_s_q.append(Mod(share["id"], p))
#         # print(id_s_q)
    
#     sec = Mod(0, p)
#     mac = Mod(0, p)
#     for i, share in enumerate(shares):
#         __temp = Mod(1, p)
#         for j in range(len(shares)):
#             if j != i:
#                 # print('a:', (id_s_q[j] - id_s_q[i]))
#                 # print('b', 1 //(id_s_q[j] - id_s_q[i]))
#                 __temp *= - id_s_q[j] // (id_s_q[i] - id_s_q[j])
#             # print(__temp)
#         # print(int(__temp))
#         # print()
#         sec += Mod(int(share['SS']), p) * int(__temp)
#         mac += Mod(int(share['SS_MAC']), p) * int(__temp)

#     print(sec)
#     print(mac)
#     print(sec ** 5)

