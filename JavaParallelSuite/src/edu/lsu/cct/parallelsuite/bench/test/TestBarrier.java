package edu.lsu.cct.parallelsuite.bench.test;

import edu.lsu.cct.parallelsuite.Barrier;
import edu.lsu.cct.parallelsuite.bench.Misc;

import java.util.LinkedList;
import java.util.concurrent.*;

public class TestBarrier {
    private static final int N = 12;
    private static final int T = 10;
    private static final ExecutorService pool = Executors.newFixedThreadPool(N, Misc.getDaemonThreadFactory());

    public static void main(String[] args) {
        var barrier = new Barrier(N);
        var futs = new LinkedList<CompletableFuture<?>>();

        for (int i = 0; i < N; i++) {
            final int threadId = i;
            futs.add(CompletableFuture.runAsync(() -> {
                for (int t = 0; t < T; t++) {
                    try {
                        Thread.sleep(ThreadLocalRandom.current().nextLong(100, 500));
                    } catch (InterruptedException ignored) { }

                    System.out.printf("March #%d by thread %d%n", t, threadId);
                    barrier.sync();
                }
            }, pool));
        }

        for (var fut : futs) {
            fut.join();
        }
    }
}
