from math import sqrt
import csv
import re
import os
from subprocess import PIPE, Popen
import sys
import argparse

parser = argparse.ArgumentParser(prog='psweet_stats', description='Create a table of PSweet Stats')
parser.add_argument('--category', type=str, default='locks', help='category to plot')
parser.add_argument('--work-size', type=int, help='work size to plot')
parser.add_argument('--num-threads', type=int, help='number of threads to plot')
pres=parser.parse_args(sys.argv[1:])

class Info:
    def __init__(self):
        p = Popen(["lscpu"],stdout=PIPE,stderr=PIPE,universal_newlines=True)
        o, e = p.communicate()
        assert p.returncode == 0
        g = re.search(r"(?m)^Model name:\s*(\S.*\S)", o)
        self.model = g.group(1)
        g = re.search(r"(?m)^CPU\(s\):\s*(\S.*\S)", o)
        self.threads = int(g.group(1))
        self.work = None
        self.machine = None

    def set_work(self, w):
        assert self.work is None or self.work == w
        self.work = w

    def set_machine(self, w):
        assert self.machine is None or self.machine == w
        self.machine = w

    def set_threads(self, w):
        assert self.threads is None or self.threads == w, f"{self.threads} != {w}"
        self.threads = w

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

info = Info()
with open('psweet.csv', 'r') as csvfile:
    reader = csv.DictReader(csvfile)
    bins = dict()
    work_size = pres.work_size
    num_threads = pres.num_threads
    for row in reader:
        if row['category'] != pres.category:
            continue

        if work_size is None:
            work_size = int(row['workPerThread'])
        if num_threads is None:
            num_threads = int(row['nThreads'])

        if int(row['workPerThread']) != work_size:
            continue
        if int(row['nThreads']) != num_threads:
            continue

        label = f"{row['machine']}:{row['workPerThread']}:{row['nThreads']}"

        info.set_machine(row['machine'])
        info.set_work(int(row['workPerThread']))
        info.set_threads(int(row['nThreads']))

        bin0 = row['specific']
        if bin0 not in bins:
            bins[bin0] = Stat()
        ms = int(row['ms'])
        bins[bin0].add(ms)

def texify(s):
    ns = ""
    for c in s:
        if c == "_":
            ns += r"\_"
        elif c in ["<", ">"]:
            ns += "$"+c+"$"
        else:
            ns += c
    return ns

with open("psweet.tex", "w") as fd:
    for bin0 in bins:
        bins[bin0].trim()

    def fk(x):
        return bins[x].stats()[0]
    sbins = sorted(list(bins.keys()), key=fk)

    print("%40s %8s %8s %4s" % (pres.category, "avg", "sdev", "n"))
    print(r"\begin{figure}",file=fd)
    print(r"\begin{tabular}{|c|c|c|}",file=fd)
    print(r"\hline",file=fd)
    print(r"\textbf{" + pres.category + r"} & \textbf{Time(ms)} & \textbf{$\sigma$(ms)} \\",file=fd)
    print(r"\hline",file=fd)
    for bin0 in sbins:
        stats = bins[bin0].stats()
        print("%40s %8.2f %8.2f %4d" % (bin0, stats[0], stats[1], stats[2]))
        print(texify(rf"{bin0} & {'%.2f' % stats[0]} & {'%.2f' % stats[1]} \\"),file=fd)
        print(r"\hline",file=fd)
    print(r"\end{tabular}",file=fd)
    print(r"\label{" + label + "}", file=fd)
    print(r"\caption{" + pres.category + " benchmark for " + info.machine + f", work: {info.work}, threads: {info.threads}, model: {info.model}"+r"}", file=fd)
    print(r"\end{figure}",file=fd)
