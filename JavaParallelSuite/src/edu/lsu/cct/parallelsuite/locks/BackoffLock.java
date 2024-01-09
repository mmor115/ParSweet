package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.atomic.AtomicBoolean;

public class BackoffLock implements SlimLock {
    private final int minDelay, maxDelay;
    private final AtomicBoolean aBool = new AtomicBoolean(false);

    public BackoffLock(int minDelay, int maxDelay) {
        this.minDelay = minDelay;
        this.maxDelay = maxDelay;
    }

    public BackoffLock() {
        this(1, 17);
    }

    @SuppressWarnings("BusyWait")
    @Override
    public void lock() {
        int delayLimit = minDelay;

        for (;;) {
            while (aBool.get()) {
                Thread.yield();
            }

            if (!aBool.getAndSet(true)) {
                return;
            } else {
                var delay = ThreadLocalRandom.current().nextInt(0, delayLimit);
                delayLimit = Math.min(maxDelay, 2 * delayLimit);
                try {
                    Thread.sleep(delay);
                } catch (InterruptedException ignored) { }
            }
        }
    }

    @Override
    public void unlock() {
        aBool.set(false);
    }
}
