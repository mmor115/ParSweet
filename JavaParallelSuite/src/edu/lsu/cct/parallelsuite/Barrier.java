package edu.lsu.cct.parallelsuite;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Supplier;

public class Barrier {
    private final int nThreads;
    private final Lock lock;
    private final Condition cond;

    private int count, march;

    public Barrier(int nThreads) {
        this(nThreads, ReentrantLock::new);
    }

    public Barrier(int nThreads, Supplier<Lock> lockSupplier) {
        this.nThreads = nThreads;
        this.count = 0;
        this.march = 0;
        this.lock = lockSupplier.get();
        this.cond = this.lock.newCondition();
    }

    public void sync() {
        lock.lock();

        try {
            count++;
            var check = march;

            if (count == nThreads) {
                count = 0;
                ++march;
                cond.signalAll();
            } else {
                while (check == march) {
                    cond.awaitUninterruptibly();
                }
            }
        } finally {
            lock.unlock();
        }
    }
}
