package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.atomic.AtomicInteger;

public class ILock implements SlimLock {
    private final AtomicInteger turn = new AtomicInteger(0);
    private final ThreadLocal<Integer> myTicket = new ThreadLocal<>();
    private static final AtomicInteger ticketCounter = new AtomicInteger(1);

    @Override
    public void lock() {
        var ticket = ticketCounter.getAndIncrement();
        myTicket.set(ticket);

        for (;;) {
            if (turn.compareAndSet(0, ticket)) {
                return;
            }

            Thread.yield();
        }
    }

    @Override
    public void unlock() {
        var ticket = myTicket.get();

        for (;;) {
            if (turn.compareAndSet(ticket, 0)) {
                return;
            }

            Thread.yield();
        }
    }
}
