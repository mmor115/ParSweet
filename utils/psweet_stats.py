from math import sqrt
import csv

class Stat:
    def __init__(self):
        self.values = []
        self.trimmed = False
    def add(self, v):
        assert not self.trimmed
        self.values.append(v)
    def trim(self):
        """ Get rid of outliers """
        assert not self.trimmed
        self.trimmed = True
        self.values = sorted(self.values)
        n = len(self.values)//10
        if n > 1:
            self.values = self.values[n:-n]
    def stats(self):
        val = 0
        val2 =  0
        n = 0
        for v in self.values:
            val += v
            val2 += v**2
            n += 1
        if n == 0:
            n = 1
        avg = val/n
        avg2 = val2/n
        sdev = sqrt(avg2-avg**2)
        return avg, sdev, n

with open('psweet.csv', 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    locks = dict()
    for row in reader:
        lock = row['specific']
        if lock not in locks:
            locks[lock] = Stat()
        ms = int(row['ms'])
        locks[lock].add(ms)

    for lock in locks:
        locks[lock].trim()

    def fk(x):
        return locks[x].stats()[0]
    slocks = sorted(list(locks.keys()), key=fk)

    print("%20s %6s %6s %3s" % ("lock", "avg", "sdev", "n"))
    for lock in slocks:
        stats = locks[lock].stats()
        print("%20s %6.2f %6.2f %3d" % (lock, stats[0], stats[1], stats[2]))
