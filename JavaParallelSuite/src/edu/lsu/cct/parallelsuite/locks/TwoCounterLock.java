package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.atomic.AtomicInteger;

public class TwoCounterLock implements SlimLock {
    private final AtomicInteger currentTicket = new AtomicInteger(0);
    private final AtomicInteger ticketDispenser = new AtomicInteger(0);

    @Override
    public void lock() {
        var myTicket = ticketDispenser.getAndIncrement();

        // wait for my ticket to come up
        while(currentTicket.get() != myTicket) {
            Thread.yield();
        }
    }

    @Override
    public void unlock() {
        // advance to the next ticket
        currentTicket.getAndIncrement();
    }
}
