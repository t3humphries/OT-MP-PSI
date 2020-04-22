"""
We output into the following format:
n=8	m	3 ...
t=2	Mean	x ...
	Min	    x ...
	Max	    x ...
	Std	    x ...
t=3	Mean    x ...
"""

import os
import json
import matplotlib.pyplot as plt
import numpy as np

# Set base-path to the folder with all the extracted json files from the reconstruction
from Graphs import paths

base_path = paths.RECON_PATH

def compute_outshape(path, scheme):
    """ We compute the width and height of the 2d matrix by looking for maximal (m,t) pairs from the filenames
    Since filenames are ambiguous, we have to scan every file
    """
    max_t, max_m = 0, 0
    for file in os.listdir(path):
        with open(os.path.join(path, file)) as json_file:
            data = json.load(json_file)
            if data["scheme"] != scheme:
                continue
            if data["m"] > max_m:
                max_m = data["m"]
            if data["t"] > max_t:
                max_t = data["t"]
    return max_t+1, max_m+1


def insert(t, m, v, matrix):
    # row, col, value, scheme
    matrix[t, m] = v


def fill_s1(data, matrix):
    m, n, r, s, t = data["m"], data["n"], data["repeat"], data["scheme"], data["t"]
    t_avg, t_max, t_min, t_std = data["s1time_avg"], data["s1time_max"], data["s1time_min"], data["s1time_std"]

    insert(4*t, m, (t_avg/1000) / 60, matrix)
    insert((4*t)+1, m, (t_min/1000)/60, matrix)
    insert((4*t)+2, m, (t_max/1000)/60, matrix)
    insert((4*t)+3, m, (t_std/1000)/60, matrix)
    return matrix


def fill_s2(data, matrix):
    print(data)
    m, n, r, s, t = data["m"], data["n"], data["repeat"], data["scheme"], data["t"]
    t_avg, t_max, t_min, t_std = data["s2time_avg"], data["s2time_max"], data["s2time_min"], data["s2time_std"]

    insert(4 * t, m, (t_avg / 1000) / 60, matrix)
    insert((4 * t) + 1, m, (t_min / 1000) / 60, matrix)
    insert((4 * t) + 2, m, (t_max / 1000) / 60, matrix)
    insert((4 * t) + 3, m, (t_std / 1000) / 60, matrix)
    return matrix

def load_data(path):
    s1_shape, s2_shape = compute_outshape(base_path, scheme=1), compute_outshape(base_path, scheme=2)

    matrix_s1 = np.zeros(shape=(5 * s1_shape[0], s1_shape[1]))
    matrix_s2 = np.zeros(shape=(5 * s2_shape[0], s2_shape[1]))

    for file in os.listdir(path):
        with open(os.path.join(path, file)) as json_file:
            data = json.load(json_file)
            s = data["scheme"]
            if s == 1:
                fill_s1(data, matrix_s1)
            else:
                fill_s2(data, matrix_s2)
    return matrix_s1, matrix_s2

def get_row(t, matrix, name="avg"):
    add = {"avg": 0, "min": 1, "max": 2, "std": 3}
    row = matrix[4*t+add[name]]
    x, y = [], []
    eps = 0.000001
    for idx, elem in enumerate(row):
        if elem >= eps:
            x.append(idx)
            y.append(elem)
    return x, y

def plot_graph(matrix_s1, matrix_s2):
    plt.title("Reconstruction Time")
    plt.xlabel("Number of Parties (m)")
    plt.ylabel("Time in Minutes")

    for s, matrix in enumerate([matrix_s1, matrix_s2]):
        for t in range(2, 100):
            row = get_row(t, matrix, "avg")
            if len(row[0]) > 0:
                plt.plot(*row, label="Scheme {}, t={}".format(s+1, t))
            else:
                break

    plt.legend()
    plt.show()

matrix_s1, matrix_s2 = load_data(base_path)
plot_graph(matrix_s1, matrix_s2)