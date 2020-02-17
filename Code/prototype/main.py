import json
import random

from utils import TrPSI_context_1, TrPSI_context_2
from ShareGen import ShareGen_1, ShareGen_2

c = TrPSI_context_1(p=10 ** 9 + 7, g=5)
x = ShareGen_1(context=c, id=3, X=2, t=3, key=4, key_mac=5, randoms=[1, 2], randoms_mac=[3, 4])
print(x)

c = TrPSI_context_2(p=10 ** 9 + 7, q=5 * (10 ** 8) + 3)
x = ShareGen_2(context=c, id=3, X=2, t=3, key=4, key_mac=5, randoms=[1, 2], randoms_mac=[3, 4])
print(x)

# test_generation
t = 10
sheme1_shares=[]
sheme2_shares=[]
randoms = [random.randint(1, c.p) for _ in range(t - 1)]
randoms_mac = [random.randint(1, c.p) for _ in range(t - 1)]
for i in range(10):
    c = TrPSI_context_1(p=10 ** 9 + 7, g=5)
    x = ShareGen_1(
        context=c, id=random.randint(1, 500), X=7, t=t, key=4, key_mac=5,
        randoms=randoms, randoms_mac=randoms_mac
    )
    sheme1_shares.append(x)
    #json.dump(x, open('test_cases/scheme_1/match/ss_X_%d.json' % i, 'w'))

    c = TrPSI_context_2(p=10 ** 9 + 7, q=5 * (10 ** 8) + 3)
    x = ShareGen_2(
        context=c, id=random.randint(1, 500), X=7, t=t, key=4, key_mac=5,
        randoms=randoms, randoms_mac=randoms_mac
    )
    sheme2_shares.append(x)
    #json.dump(x, open('test_cases/scheme_2/match/ss_X_%d.json' % i, 'w'))
json.dump(sheme1_shares, open('test_cases/ss1_match.json','w'))
json.dump(sheme2_shares, open('test_cases/ss2_match.json','w'))

sheme1_shares=[]
sheme2_shares=[]
randoms = [random.randint(1, c.p) for _ in range(t - 1)]
randoms_mac = [random.randint(1, c.p) for _ in range(t - 1)]
for i in range(5):
    c = TrPSI_context_1(p=10 ** 9 + 7, g=5)
    x = ShareGen_1(
        context=c, id=random.randint(1, 500), X=7, t=t, key=4, key_mac=5,
        randoms=randoms, randoms_mac=randoms_mac
    )
    y = ShareGen_1(
        context=c, id=random.randint(1, 500), X=10, t=t, key=4, key_mac=5,
        randoms=randoms, randoms_mac=randoms_mac
    )
    sheme1_shares.append(x)
    sheme1_shares.append(y)
    #json.dump(x, open('test_cases/scheme_1/unmatch/ss_X_%d.json' % i, 'w'))
    #json.dump(y, open('test_cases/scheme_1/unmatch/ss_Y_%d.json' % i, 'w'))

    c = TrPSI_context_2(p=10 ** 9 + 7, q=5 * (10 ** 8) + 3)
    x = ShareGen_2(
        context=c, id=random.randint(1, 500), X=7, t=t, key=4, key_mac=5,
        randoms=randoms, randoms_mac=randoms_mac
    )
    y = ShareGen_2(
        context=c, id=random.randint(1, 500), X=10, t=t, key=4, key_mac=5,
        randoms=randoms, randoms_mac=randoms_mac
    )
    sheme2_shares.append(x)
    sheme2_shares.append(y)
    #json.dump(x, open('test_cases/scheme_2/unmatch/ss_X_%d.json' % i, 'w'))
    #json.dump(y, open('test_cases/scheme_2/unmatch/ss_Y_%d.json' % i, 'w'))
json.dump(sheme1_shares, open('test_cases/ss1_unmatched.json','w'))
json.dump(sheme2_shares, open('test_cases/ss2_unmatched.json','w'))