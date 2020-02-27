import math
import random
from mod import Mod


def ShareGen_1(context, id, X, t, key, key_mac, randoms, randoms_mac):
    if len(randoms) + 1 != t:
        print("Insufficient Randomness")
        return None

    g = Mod(context.g, context.p)

    ############## Element Holder ##############

    h_x = Mod(context.h(X), context.p)
    # h_x = Mod(2, context.p) # For testing, set this value

    alpha = random.randint(1, (context.p - 1))  # relatively prime to p-1
    while math.gcd(alpha, context.p - 1) > 1:
        alpha = random.randint(1, (context.p - 1))  # relatively prime to p-1
    alpha_inv = (1 // Mod(alpha, context.p - 1)).__int__()

    h_x_alpha = h_x ** alpha
    g_alpha = g ** alpha

    ############## Key Holder ##############

    r = random.randint(1, (context.p - 2))
    R = g ** r
    R_inverse = 1 // R
    R_alpha = g_alpha ** r

    masked_secret_alpha = R_alpha * (h_x_alpha ** key)
    masked_coefficients_alpha = [R_alpha * (h_x_alpha ** rr) for rr in randoms]

    masked_mac_alpha = R_alpha * (h_x_alpha ** (key * key_mac))
    masked_mac_coefficients_alpha = [R_alpha * (h_x_alpha ** rr) for rr in randoms_mac]

    ############## Element Holder ##############

    masked_secret = (masked_secret_alpha ** alpha_inv).__int__()
    masked_coefficients = [(x ** alpha_inv).__int__() for x in masked_coefficients_alpha]

    enc_masked_secret = context.pk.encrypt(masked_secret)
    enc_masked_coefficients = [context.pk.encrypt(x) for x in masked_coefficients]

    masked_mac = (masked_mac_alpha ** alpha_inv).__int__()
    masked_mac_coefficients = [(x ** alpha_inv).__int__() for x in masked_mac_coefficients_alpha]

    enc_masked_mac = context.pk.encrypt(masked_mac)
    enc_masked_mac_coefficients = [context.pk.encrypt(x) for x in masked_mac_coefficients]

    ############## Key Holder ##############

    enc_secret = enc_masked_secret * R_inverse.__int__()
    enc_coefficients = [x * R_inverse.__int__() for x in enc_masked_coefficients]

    enc_mac = enc_masked_mac * R_inverse.__int__()
    enc_mac_coefficients = [x * R_inverse.__int__() for x in enc_masked_mac_coefficients]

    id_pows = Mod(id, context.p)

    for i in range(len(enc_masked_coefficients)):
        enc_secret = enc_secret + enc_coefficients[i] * id_pows.__int__()
        enc_mac = enc_mac + enc_mac_coefficients[i] * id_pows.__int__()
        id_pows = id_pows * id

    ############## Element Holder ##############

    dec_part_k_1_unmasked = context.sk.decrypt(enc_secret)
    dec_part_k_2_unmasked = context.sk.decrypt(enc_mac)

    return {
        "id": id,
        "bin": context.h_b(X),
        "SS": dec_part_k_1_unmasked % context.p,
        "SS_MAC": dec_part_k_2_unmasked % context.p
    }


def ShareGen_2(context, id, X, t, key, key_mac, randoms, randoms_mac, alpha=None):
    if len(randoms) + 1 != t:
        print("Insufficient Randomness")
        return None

    ############## Element Holder ##############
    if alpha != None:
        h_x = Mod(context.h(X), context.p)
        h_x = h_x * h_x
        # h_x = Mod(2, context.p) # For testing, set this value

        alpha = random.randint(1, context.q).__int__()  # relatively prime to q
        alpha_inv = (1 // Mod(alpha, context.q)).__int__()

        h_x_alpha = h_x ** alpha.__int__()
    
    else:
        h_x_alpha = Mod(context.h(X), context.p)


    ############## Key Holder ##############

    coeffs_sum = Mod(key, context.q)
    coeffs_mac_sum = Mod(key_mac, context.q)
    id_pows = Mod(id, context.q)

    for i in range(len(randoms)):
        coeffs_sum += id_pows * randoms[i]
        coeffs_mac_sum += id_pows * randoms_mac[i]
        id_pows = id_pows * id

    secret_alpha = h_x_alpha ** coeffs_sum.__int__()
    secret_mac_alpha = h_x_alpha ** coeffs_mac_sum.__int__()

    ############## Element Holder ##############

    if alpha != None:
        secret = (secret_alpha ** alpha_inv).__int__()
        secret_mac = (secret_mac_alpha ** alpha_inv).__int__()

    else:
        secret = secret_alpha
        secret_mac = secret_mac_alpha

    return {
        "id": id,
        "bin": context.h_b(X),
        "SS": secret,
        "SS_MAC": secret_mac
    }
