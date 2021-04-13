import numpy as np
import scipy.optimize as spo
import random
import sys
import time

def read_data():
    with open('dataset_328552_4.txt') as input:
        sys.stdin = input
        n, p, k, M = map(int, sys.stdin.readline().split())
        x = np.ndarray((n, 2))
        money = np.ndarray(n)
        for i in range(n):
            row = list(map(float, sys.stdin.readline().split()))
            x[i] = [row[0], row[1]]
            money[i] = row[2]

    return n, p, k, M, x, money

def NN(n, x, starting_point=0):
    st = np.zeros(n, dtype=bool)
    ans = list([0] * n)
    cur = starting_point
    st[cur] = True
    ans[0] = starting_point
    for j in range(1, n):
        dists = np.linalg.norm(x[cur] - x, axis=1)
        dists[st] = np.inf
        cur_min_index = np.argmin(dists)
        ans[j] = cur_min_index
        if cur_min_index >= n:
            return np.flip(np.arange(n))
        st[cur_min_index] = True
        cur = cur_min_index
    return ans

## NN not wasteful!
def NNNF(n, x, starting_point=0):
    st = np.zeros(n, dtype=bool)
    ans = list([0] * n)
    cur = starting_point
    st[cur] = True
    ans[0] = starting_point
    for j in range(1, n):
        dists = np.linalg.norm(x[cur] - x, axis=1)
        dists[st] = np.inf
        cur_min_index = np.argmin(dists)
        ans[j] = cur_min_index
        if cur_min_index >= n:
            return np.flip(np.arange(n))
        st[cur_min_index] = True
        cur = cur_min_index
    return ans

# best neighboor not drunkard!
def BNND(n, p, k, M, x, money, starting_point=0):
    # build (k-1)-path first
    st = np.zeros(n, dtype=bool)
    cur_stole = 0
    ans = list([0] * n)
    cur = starting_point
    st[cur] = True
    ans[0] = starting_point
    for j in range(1, k-1):
        dists = np.linalg.norm(x[cur] - x, axis=1)
        dists[st] = np.inf
        cur_min_index = np.argmin(dists)
        ans[j] = cur_min_index
        if cur_min_index >= n:
            return np.flip(np.arange(n))
        st[cur_min_index] = True
        cur_stole += money[cur_min_index]
        cur = cur_min_index
    
    # now keep going greedy without getting drunk
    for j in range(k-1, n):
        dists = np.linalg.norm(x[cur] - x, axis=1)
        dists[st] = np.inf
        cur_min_index = -1
        cur_dist = np.inf
        for i in range(n):
            if dists[i] < cur_dist and money[i] + cur_stole <= M:
                cur_min_index = i
                cur_dist = dists[i]
        if cur_min_index < 0:
            return ans[:j]
        ans[j] = cur_min_index
        st[cur_min_index] = True
        cur_stole += money[cur_min_index]
        cur_stole -= money[ans[j - k + 1]]
        cur = cur_min_index
    
    return ans

def dist(p1, p2):
    return np.linalg.norm(p1 - p2)

def NI(n, x):
    shortest_length = np.inf
    shortest_edge = [None, None]
    next_v = np.zeros(n, dtype=int)
    next_v -= 1

    # shortest edge
    for i in range(n):
        for j in range(i + 1, n):
            if dist(x[i], x[j]) < shortest_length:
                shortest_length = dist(x[i], x[j])
                shortest_edge = [i, j]
    
    # smallest triangle with The edge
    best_insertion = -1
    best_insertion_length = np.inf
    for i in range(n):
        if i != shortest_edge[0] and i != shortest_edge[1]:
            if dist(i, shortest_edge[0]) + dist(i, shortest_edge[1]) < best_insertion_length:
                best_insertion_length = dist(i, shortest_edge[0]) + dist(i, shortest_edge[1])
                best_insertion = i
    
    # make initial triangle
    next_v[shortest_edge[0]] = shortest_edge[1]
    next_v[shortest_edge[1]] = best_insertion
    next_v[best_insertion] = shortest_edge[0]

    # NI
    for _ in range(n - 3):
        best_insertion = -1
        best_insertion_length = np.inf
        insertion_point = i
        for j in range(n):
            if next_v[j] == -1:
                for i in range(n):
                    if next_v[i] >= 0:
                        if dist(i, j) + dist(next_v[i], j) < best_insertion_length:
                            best_insertion_length = dist(i, j) + dist(next_v[i], j) 
                            best_insertion = j
                            insertion_point = i
        next_v[j] = next_v[insertion_point]
        next_v[insertion_point] = j
    
    solution = list([0] * n)
    cur_point = 0
    for i in range(n):
        solution[i] = int(cur_point)
        cur_point = next_v[cur_point]
    return solution

def reconnect_2(solution, i, j):
    solution[i:j] = solution[i:j][::-1]

def substitute_2_local_search(x, solution):
    global start_time
    n = len(solution)
    while time.time() - start_time < 300:
        is_changed = False
        for i in range(1, n):
            for j in range(i + 1, n):
                e1p1 = x[solution[i - 1]]
                e1p2 = x[solution[i]]
                e2p1 = x[solution[j - 1]]
                e2p2 = x[solution[j]]

                if dist(e1p1, e1p2) + dist(e2p1, e2p2) > dist(e1p1, e2p1) + dist(e1p2, e2p2):
                    is_changed = True
                    reconnect_2(solution, i, j)
        if not is_changed:
            return

def skip_wasteful_islands(ans, p, x, money):
    new_ans = list([ans[0]])
    for i in range(1, len(ans) - 1):
        if - p * dist(x[new_ans[-1]], x[ans[i + 1]]) < money[ans[i]] - p * (dist(x[new_ans[-1]], x[ans[i]]) + dist(x[ans[i]], x[ans[i + 1]])):
            new_ans.append(ans[i])
    new_ans.append(ans[0])
    return new_ans

start_time = time.time()
n, p, k, M, x, money = read_data()

#ans = BNND(n, p, k, M, x, money)
ans = NN(n, x)

substitute_2_local_search(x, ans)
ans.append(ans[0])
for _ in range(50):
    ans = skip_wasteful_islands(ans, p, x, money)

substitute_2_local_search(x, ans)

with open('output.txt', mode='w') as output: 
    ssss = sys.stdout
    sys.stdout = output
    for i in range(len(ans)):
        print(ans[i] + 1, end=' ')

    sys.stdout = ssss

print ("Finished TSP")