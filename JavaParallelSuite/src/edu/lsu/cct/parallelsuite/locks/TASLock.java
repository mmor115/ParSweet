package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.atomic.AtomicBoolean;

public class TASLock implements SlimLock {
    private final AtomicBoolean aBool = new AtomicBoolean(false);

    @Override
    public void lock() {
        while (aBool.getAndSet(true)) {
            Thread.yield();
        }
    }

    @Override
    public void unlock() {
        aBool.set(false);
    }
}
