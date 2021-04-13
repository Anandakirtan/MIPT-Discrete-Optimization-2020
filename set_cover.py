import numpy as np
import scipy.optimize as spo
import random
import sys


def read_data():
    n_columns, n_rows = map(int, input().split())
    M = np.zeros((n_rows, n_columns)) # матрица покрытий
    w = np.zeros(n_rows) # матрица весов

    i = 0
    for line in sys.stdin:
        row = list(map(int, line.split()))
        w[i] = row[0]
        for j in range(1, len(row)):
            M[i][row[j]] = 1
        i += 1
        if i >= n_rows:
            break
    return M, w

def solve_LP(M, w):
    c = w
    A = -M.T
    b = -np.ones(len(M.T))
    indicator_bounds = np.array([np.zeros(len(w)), np.ones(len(w))]).T
    res = spo.linprog(c, A_ub=A, b_ub=b, bounds=indicator_bounds)
    return res['x']

def round(M, x):
    x_rounded = np.zeros(len(x))
    b = np.ones(len(M.T))
    
    # ordering, so we flip most probable first
    order = np.flip(np.argsort(x), axis=0)
    # some kostyli's for speed
    mask = np.ones(len(x), dtype=bool)
    # yeah, it's kinda the same array ;)

    # first round of rounding
    for i in range(len(x)):
        if not x_rounded[i]:
            coin = random.random()
            if coin < x[i]:
                x_rounded[i] = 1
                mask[i] = False # we won't flip this coin anymore

    # while we haven't covered everything
    while (M.T @ x_rounded < b).any():
        for i in order[mask]:
            coin = random.random()
            if coin < x[i]:
                x_rounded[i] = 1
                mask[i] = False
                # breaking to check cover after each row addition
                break
    
    return x_rounded

M, w = read_data()
x_0 = solve_LP(M,w)
x = round(M, x_0)
for i in range(len(x)):
    if x[i]:
        print(i + 1, end=' ')