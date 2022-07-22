import os
import sys
import time
from timeit import default_timer as timer
from multiprocessing import Process, process


def run_process(REPEAT, PORT, SLEEP):
    os.system(f"./process.out {REPEAT} {PORT} {SLEEP}")


def run_n_process(N, REPEAT, SLEEP):
    PORT = 2022
    process_list = []
    start = time.time()
    for i in range(N):
        p = Process(target=run_process, args=(REPEAT, PORT, SLEEP))
        process_list.append(p)
        p.start()
        PORT += 1
    for p in process_list:
        p.join()
    stop = time.time()
    elapsed_time = stop - start
    print("N = " + str(N) + " Time: " + str(elapsed_time))


def run_test(n, r, k):
    for i in n:
        run_n_process(i, r, k)


if __name__ == '__main__':
    if len(sys.argv) > 1:
        if sys.argv[1] == "1":
            n = [2, 4, 8, 16, 32]
            r = 10
            k = 2
            run_test(n, r, k)

        if sys.argv[1] == "2":
            n = [2, 4, 8, 16, 32, 64]
            r = 5
            k = 1
            run_test(n, r, k)

        if sys.argv[1] == "3":
            n = [2, 4, 8, 16, 32, 64, 128]
            r = 3
            k = 0
            run_test(n, r, k)
    else:
        n = [2]
        r = 10
        k = 2
        run_test(n, r, k)
            
