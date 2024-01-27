package edu.lsu.cct.parallelsuite.bench.test;

import edu.lsu.cct.parallelsuite.bench.Misc;
import edu.lsu.cct.parallelsuite.maps.SetBasedMap;
import edu.lsu.cct.parallelsuite.sets.FineGrainedSet;
import edu.lsu.cct.parallelsuite.sets.LazySet;
import edu.lsu.cct.parallelsuite.sets.OptimisticSet;

import java.util.LinkedList;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Supplier;

public class TestMaps {
    private static final int THREADS = 12;
    private static final int WORK_SIZE = 1000 / THREADS;

    private static final ExecutorService pool = Executors.newFixedThreadPool(THREADS, Misc.getDaemonThreadFactory());

    private static abstract class Controller<K, V> {
        protected final Map<K, V> mapImpl;

        public Controller(Supplier<Map<K, V>> mapSupplier) {
            this.mapImpl = mapSupplier.get();
        }

        public abstract void test();
    }

    private static class ControllerA extends Controller<Integer, Integer> {
        public ControllerA(Supplier<Map<Integer, Integer>> mapSupplier) {
            super(mapSupplier);
        }

        @Override
        public void test() {
            var futs = new LinkedList<CompletableFuture<?>>();

            for (int t = 0; t < THREADS; t++) {
                final var threadId = t;

                futs.add(CompletableFuture.runAsync(() -> {
                    boolean x;

                    for (int i = 0; i < WORK_SIZE; i++) {
                        var token = i * THREADS + threadId;

                        mapImpl.put(token, token);
                        assert Objects.equals(token, mapImpl.get(token));
                        mapImpl.remove(token);
                        assert Objects.isNull(mapImpl.get(token));
                    }

                    for (int i = 0; i < WORK_SIZE; i++) {
                        var token = i * THREADS + threadId;
                        mapImpl.put(token, token);
                    }

                    for (int i = 0; i < WORK_SIZE; i++) {
                        var token = i * THREADS + threadId;
                        assert Objects.equals(token, mapImpl.get(token));
                    }

                    for (int i = 0; i < WORK_SIZE; i++) {
                        mapImpl.remove(i * THREADS + threadId);
                    }

                    for (int i = 0; i < WORK_SIZE; i++) {
                        var token = i * THREADS + threadId;
                        assert Objects.isNull(mapImpl.get(token));
                    }
                }, pool));
            }

            for (var fut : futs) {
                fut.join();
            }
        }
    }

    private static void testMap(Supplier<Map<Integer, Integer>> intMapSupplier) {
        Controller<?, ?> testA = new ControllerA(intMapSupplier);

        testA.test();
    }

    private static void testMaps() {
        // Sanity check: Standard Java implementation
        testMap(ConcurrentHashMap::new);

        testMap(() -> new SetBasedMap<>(FineGrainedSet::new));
        testMap(() -> new SetBasedMap<>(OptimisticSet::new));
        testMap(() -> new SetBasedMap<>(LazySet::new));
    }

    private static final int TIMES = 5;

    public static void main(String[] args) {
        boolean assertionsOn = false;
        assert assertionsOn = true;
        System.out.printf("Assertions are %s.%n", assertionsOn ? "ON" : "OFF");

        for (int i = 0; i < TIMES; i++) {
            testMaps();
        }
    }
}
