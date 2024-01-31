from math import sqrt
import csv

class Stat:
    def __init__(self):
        self.val = 0
        self.val2 = 0
        self.n = 0
    def add(self, v):
        self.val += v
        self.val2 += v**2
        self.n += 1
    def stats(self):
        avg = self.val/self.n
        avg2 = self.val2/self.n
        sdev = sqrt(avg2-avg**2)
        return avg, sdev, self.n

with open('psweet.csv', 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    locks = dict()
    for row in reader:
        lock = row['specific']
        if lock not in locks:
            locks[lock] = Stat()
        ms = int(row['ms'])
        locks[lock].add(ms)
    def fk(x):
        return locks[x].stats()[0]
    slocks = sorted(list(locks.keys()), key=fk)
    print("%20s %6s %6s %3s" % ("lock", "avg", "sdev", "n"))
    for lock in slocks:
        stats = locks[lock].stats()
        print("%20s %6.2f %6.2f %3d" % (lock, stats[0], stats[1], stats[2]))
