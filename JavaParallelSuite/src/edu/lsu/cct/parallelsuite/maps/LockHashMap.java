package edu.lsu.cct.parallelsuite.maps;

import java.util.concurrent.locks.Lock;
import java.util.function.Supplier;

public class LockHashMap<K, V> {
    private static final int N_BUCKETS = 16;

    private final Object/*NodeHead*/[] buckets;
    private final Supplier<Lock> lockSupplier;

    public LockHashMap(Supplier<Lock> lockSupplier) {
        this.lockSupplier = lockSupplier;

        buckets = new Object[N_BUCKETS];
        for (int i = 0; i < N_BUCKETS; ++i) {
            buckets[i] = new NodeHead();
        }
    }

    @SuppressWarnings("unchecked")
    NodeHead getNodeHead(int index) {
        return ((NodeHead)buckets[index]);
    }

    public V put(K key, V value) {
        var index = getBucketIndexOfKey(key);
        var node = new Node(key, value);

        var nodeHead = getNodeHead(index);

        nodeHead.lock.lock();
        try {
            var h = nodeHead.head;
            if (h == null) {
                nodeHead.head = node;
            } else {
                Node current = h;

                while (true) {
                    if (current.key.equals(key)) {
                        var old = current.value;
                        current.value = value;
                        return old;
                    }

                    if (current.next != null) {
                        current = current.next;
                    } else {
                        break;
                    }
                }

                current.next = node;
            }
        } finally {
            nodeHead.lock.unlock();
        }

        return null;
    }

    public V get(K key) {
        var index = getBucketIndexOfKey(key);
        var nodeHead = getNodeHead(index);

        nodeHead.lock.lock();
        try {
            var h = nodeHead.head;
            if (h == null) {
                return null;
            }

            Node current = h;

            while (current != null) {
                if (current.key.equals(key)) {
                    return current.value;
                }
                current = current.next;
            }

            return null;
        } finally {
            nodeHead.lock.unlock();
        }
    }

    public boolean delete(K key) {
        var index = getBucketIndexOfKey(key);
        var nodeHead = getNodeHead(index);

        nodeHead.lock.lock();
        try {
            var h = nodeHead.head;

            if (h == null) {
                return false;
            }

            Node current = h, prev = null;
            while (current != null) {
                if (current.key.equals(key)) {
                    if (prev == null) {
                        nodeHead.head = current.next;
                    } else {
                        prev.next = current.next;
                    }

                    return true;
                }

                prev = current;
                current = current.next;
            }

            return false;
        } finally {
            nodeHead.lock.unlock();
        }
    }

    private class Node {
        private final K key;
        private V value;
        private Node next;

        private Node(K key, V value) {
            this.key = key;
            this.value = value;
        }
    }

    private class NodeHead {
        private final Lock lock;
        private Node head;

        private NodeHead() {
            this.lock = lockSupplier.get();
            this.head = null;
        }
    }

    private int getBucketIndexOfKey(K key) {
        return key.hashCode() % (N_BUCKETS - 1);
    }
}
