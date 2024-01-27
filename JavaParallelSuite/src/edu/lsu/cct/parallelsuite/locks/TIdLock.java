package edu.lsu.cct.parallelsuite.locks;

import edu.lsu.cct.parallelsuite.ThreadId;

import java.util.concurrent.atomic.AtomicInteger;

public class TIdLock implements SlimLock {
    private final AtomicInteger turn = new AtomicInteger(0);

    @Override
    public void lock() {
        var ticket = ThreadId.get();

        for (;;) {
            if (turn.get() == 0 && turn.compareAndSet(0, ticket)) {
                return;
            }

            Thread.yield();
        }
    }

    @Override
    public void unlock() {
        var ticket = ThreadId.get();

        boolean worked = turn.compareAndSet(ticket, 0);
        assert worked;
    }
}
