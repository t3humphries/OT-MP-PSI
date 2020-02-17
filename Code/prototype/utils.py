import phe
import hashlib


def def_hash_(mod=None):
    def hash_(X):
        h = hashlib.sha256()
        h.update(bytes(str(X), 'utf-8'))
        ans = int.from_bytes(h.digest(), byteorder='big')
        if mod is not None:
            return (ans * ans) % mod
        else:
            return ans * ans
    return hash_


def hash_bin(X, bins=10):
    h = hashlib.sha256()
    h.update(bytes(str(X), 'utf-8'))
    return int.from_bytes(h.digest(), byteorder='big') % bins


class TrPSI_context_1:
    def __init__(self, p, g, h=None, h_b=hash_bin):
        self.p = p
        self.g = g
        self.h = h
        if self.h is None:
            self.h = def_hash_(mod=p)
        self.h_b = h_b
        self.pk, self.sk = phe.paillier.generate_paillier_keypair(n_length=2 * len(bin(p)) + 80)


class TrPSI_context_2:
    def __init__(self, p, q, h=None, h_b=hash_bin):
        if p != 2 * q + 1:
            raise KeyError()
        self.p = p
        self.q = q
        self.h = h
        self.h = h
        if self.h is None:
            self.h = def_hash_(mod=p)
        self.h_b = h_b


