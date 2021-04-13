import numpy as np
import scipy.optimize as spo
import random
import sys
from copy import deepcopy, copy
import time

def read_data():
    # with open("input.txt") as inp:
    #     sys.stdin = inp
    i = -1
    for line in sys.stdin:
        if i < 0:
            n_columns, n_rows = list(map(int, line.split()))
            rows = list()
            w = np.zeros(n_rows, dtype="int8") # матрица весов
            i += 1
        else:
            row = list(map(int, line.split()))
            w[i] = row[0]
            rows.append(row[1:])
            i += 1
            if i >= n_rows:
                break
    return rows, w, n_columns

def not_covered(rows, active:list, n_columns):
    is_covered = np.zeros(n_columns, dtype=bool)
    for i in active:
        for j in rows[i]:
            is_covered[j] = True
    if is_covered.all():
        return False
    else:
        return True

def random_rounding_cover(rows, n_columns, active:list):
    order = list(range(len(rows)))
    random.shuffle(order)
    # while we haven't covered everything
    while not_covered(rows, active, n_columns):
        for i in order:
            if i not in active:
                coin = random.random()
                if coin < 0.3:
                    active.append(i)
                    # breaking to check cover after each row addition
                    break
    
    return active

def fusion(rows, n_columns, parent1, parent2, fitness1, fitness2):
    child = []
    for i in parent1:
        if i in parent2:
            coin = random.random()
            child.append(i)
        else:
            # child takes after the better parent (somewhat) (better means more fit, don't be cynical, that's just an algorithm)
            coin = random.random()
            if coin + 0.05 < fitness1 / (fitness1 + fitness2):
                child.append(i)

    for j in parent2:
        if j not in parent1:
            coin = random.random()
            if coin + 0.05 < fitness2 / (fitness1 + fitness2):
                child.append(j)

    # now the child is formed more or less, but it might be deficient -- not a cover, that is 
    # (damn, that's sad(can't we use some other terminology, besides parents and children?))
    # anyway, polish(ew!) the child and make him a cover (yeah, let's impose some expexctations on pure soul)
    # but a child is stupid and greedy (he won't be someday(trust me, I just turned 22, I know, what I'm talking about!))
    # or not -- not anymore, I changed the code, but I'm leaving this wisdom here, because it's beatiful
    # also mutate a bit, because why not
    return random_rounding_cover(rows, n_columns, child)


def SCP():
    rows, w, n_columns = read_data()
    population = []

    start_time = time.perf_counter()
    for i in range(10):
        # the solutions will be somewhat different, because of random elements
        population.append(random_rounding_cover(rows, n_columns, []))
    
    fitness = np.zeros(len(population))
    for i in range(len(population)):
        for j in population[i]:
            fitness[i] += w[j]

    while time.perf_counter() - start_time < 29:
        unfit = np.argmax(fitness)

        i = random.randint(0, 9)
        j = random.randint(0, 9)
        population[unfit] = fusion(rows, n_columns, population[i], population[j], fitness[i], fitness[j])
        fitness[unfit] = 0
        for k in population[unfit]:
            fitness[unfit] += w[k]
    
    # choose the fittest
    fitness = np.zeros(len(population))
    for i in range(len(population)):
        for j in population[i]:
            fitness[i] += w[j]
    ind = np.argmin(fitness)
    champion = population[ind]

    # give him to the world
    for i in champion:
        print(i + 1, end=' ')

SCP()