package edu.lsu.cct.parallelsuite.sets;

import java.util.Objects;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Supplier;

public class LazySet<T> implements SlimSet<T> {
    private final Node head;
    private final Supplier<Lock> lockSupplier;

    public LazySet(Supplier<Lock> lockSupplier) {
        this.lockSupplier = lockSupplier;
        this.head = new Node();
        head.key = Integer.MIN_VALUE;
        head.next = new Node();
        head.next.key = Integer.MAX_VALUE;
    }

    public LazySet() {
        this(ReentrantLock::new);
    }

    private boolean find(T t, FindCallback<Node> callback) {
        final var key = t.hashCode();

        for (;;) {
            var predecessor = head;
            var current = predecessor.next;

            while (current.next != null
                    && current.key <= key
                    && (key != current.key || !Objects.equals(t, current.value))) {
                predecessor = current;
                current = current.next;
            }

            try {
                predecessor.lock();
                current.lock();

                if (!predecessor.deleted && !current.deleted && predecessor.next == current) {
                    return callback.onFound(predecessor, current);
                }
            } finally {
                current.unlock();
                predecessor.unlock();
            }
        }
    }

    @Override
    public boolean contains(Object o) {
        return find((T) o, (predecessor, current) -> Objects.equals(o, current.value));
    }

    @Override
    public T getEqual(final T t) {
        var ref = new Object() {
            T found = null;
        };

        find(t, (predecessor, current) -> {
            if (!Objects.equals(t, current.value)) {
                return false;
            }

            ref.found = current.value;
            return true;
        });

        return ref.found;
    }

    @Override
    public boolean add(final T t) {
        return find(t, (predecessor, current) -> {
            if (Objects.equals(t, current.value)) {
                return false;
            }
            var newNode = new Node(t);
            newNode.next = current;
            predecessor.next = newNode;
            return true;
        });
    }

    @Override
    public boolean remove(Object o) {
        return find((T) o, (predecessor, current) -> {
            if (!Objects.equals(o, current.value)) {
                return false;
            }
            current.deleted = true;
            predecessor.next = current.next;
            return true;
        });
    }

    private class Node {
        private final Lock lock;
        private int key;
        private final T value;
        private Node next;
        private boolean deleted;

        public Node(T value) {
            this.lock = lockSupplier.get();
            this.value = value;
            if (value != null) {
                this.key = value.hashCode();
            }
        }

        public Node() {
            this(null);
        }

        public void lock() {
            lock.lock();
        }

        public void unlock() {
            lock.unlock();
        }
    }
}
