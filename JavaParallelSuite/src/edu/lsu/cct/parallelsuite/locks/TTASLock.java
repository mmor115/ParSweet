package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.atomic.AtomicBoolean;

public class TTASLock implements SlimLock {
    private final AtomicBoolean aBool = new AtomicBoolean(false);

    @Override
    public void lock() {
        for (;;) {
            if (!aBool.get() && !aBool.getAndSet(true)) {
                return;
            }

            Thread.yield();
        }
    }

    @Override
    public void unlock() {
        aBool.set(false);
    }
}
