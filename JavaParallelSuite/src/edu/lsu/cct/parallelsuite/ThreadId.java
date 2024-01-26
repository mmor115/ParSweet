package edu.lsu.cct.parallelsuite;

import java.util.concurrent.atomic.AtomicInteger;

public final class ThreadId {
    private static final AtomicInteger idCounter = new AtomicInteger(1);
    private static final ThreadLocal<Integer> id = ThreadLocal.withInitial(() -> 0);

    public static int get() {
        if (id.get() == 0) {
            id.set(idCounter.getAndIncrement());
        }

        assert id.get() != 0;
        return id.get();
    }
}
