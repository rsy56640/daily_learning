import numpy as np
import k_means
from functools import reduce


def dist(lhs: np.array, rhs: np.array) -> float:
    diff = lhs - rhs
    d = 0.0
    for i in range(diff.shape[0]):
        d += diff[i] ** 2
    return np.sqrt(d)


def test():
    data_set = []
    data_set.append(np.array([1.2, 2.3, 3.14]))
    data_set.append(np.array([1.3, 3.01, 4.0]))
    data_set.append(np.array([1.4, 3.1, 3.22]))
    data_set.append(np.array([1.5, 2.3, 3.64]))
    data_set.append(np.array([1.6, 2.33, 4.2]))
    data_set.append(np.array([1.7, 2.7, 3.76]))
    data_set.append(np.array([1.8, 2.3, 3.72]))
    data_set.append(np.array([1.88, 3.3, 4.1]))
    data_set.append(np.array([1.9, 3.0, 3.95]))

    data_set.append(np.array([7.2, 8.1, 19.99]))
    data_set.append(np.array([7, 8.24, 20]))
    data_set.append(np.array([7.54, 7.9, 18.5]))
    data_set.append(np.array([7.25, 8.1, 19.8]))
    data_set.append(np.array([7, 8.24, 20]))
    data_set.append(np.array([7.77, 7.6, 18.6]))
    data_set.append(np.array([7.66, 8.2, 19.6]))
    data_set.append(np.array([7, 8.24, 20]))
    data_set.append(np.array([7.52, 7.8, 18.2]))

    data_set.append(np.array([11.8, 21.7, -34.1]))
    data_set.append(np.array([12.1, 20.7, -33.6]))
    data_set.append(np.array([10.4, 19.4, -33.8]))
    data_set.append(np.array([11.9, 22.5, -34.2]))
    data_set.append(np.array([12.1, 20.7, -33.6]))
    data_set.append(np.array([10.9, 19.8, -32.9]))
    data_set.append(np.array([11.2, 22.0, -34.3]))
    data_set.append(np.array([12.2, 20.9, -43.0]))
    data_set.append(np.array([10.5, 19.9, -23.5]))

    C = k_means.k_means(3, dist, *data_set)

    # output
    for cluster in C:
        for vec in cluster:
            print(vec)
        mean_vec = reduce(lambda x, y: x + y, cluster) / len(cluster)
        print("The mean vector is:", mean_vec)
        print()


test()
