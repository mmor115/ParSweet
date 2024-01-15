package edu.lsu.cct.parallelsuite.maps;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.Supplier;

public class LockTreeMap<K extends Comparable<? super K>, V> {
    private volatile Node head;

    private final Supplier<Lock> lockSupplier;

    public LockTreeMap() {
        this(ReentrantLock::new);
    }

    public LockTreeMap(Supplier<Lock> lockSupplier) {
        this.lockSupplier = lockSupplier;
    }

    public V get(K key) {
        return get(key, head);
    }

    private V get(K key, Node node) {
        if (node == null) {
            return null;
        }

        node.selfLock.lock();

        Node toVisit;
        Lock toUnlock;

        try {
            var cmp = key.compareTo(node.key);

            if (cmp == 0) {
                return node.value;
            } else if (cmp < 0) {
                toVisit = node.left;
                (toUnlock = node.leftLock).lock();
            } else {
                toVisit = node.right;
                (toUnlock = node.rightLock).lock();
            }
        } finally {
            node.selfLock.unlock();
        }

        assert toVisit != null;
        assert toUnlock != null;

        try {
            return get(key, toVisit);
        } finally {
            toUnlock.unlock();
        }
    }

    public void put(K key, V val) {
        var newNode = new Node(key, val);

        var parent = findInsertionParent(key, head, null);
        if (parent == null) {
            head = newNode;
        } else {
            try {
                if (key.compareTo(parent.key) < 0) {
                    parent.leftLock.lock();
                    try {
                        parent.left = newNode;
                    } finally {
                        parent.leftLock.unlock();
                    }
                } else {
                    parent.rightLock.lock();
                    try {
                        parent.right = newNode;
                    } finally {
                        parent.rightLock.unlock();
                    }
                }
            } finally {
                parent.selfLock.unlock();
            }
        }
    }

    public V delete(K key) {
        Node n = delete(key, head);
        return n == null ? null
                         : n.value;
    }

    private Node min(Node node) {
        node.selfLock.lock();
        node.leftLock.lock();

        try {
            if (node.left == null) {
                return node;
            } else {
                return min(node.left);
            }
        } finally {
            node.leftLock.unlock();
            node.selfLock.unlock();
        }
    }

    private Node delete(K key, Node node) {
        if (node == null) {
            return null;
        }

        Node toVisit;
        Lock toUnlock;
        int cmp;

        node.selfLock.lock();
        try {
            cmp = key.compareTo(node.key);

            if (cmp < 0) {
                toVisit = node.left;
                (toUnlock = node.leftLock).lock();
            } else if (cmp > 0) {
                toVisit = node.right;
                (toUnlock = node.rightLock).lock();
            } else {
                var nodeLeft = node.left;
                var nodeRight = node.right;

                if (nodeLeft == null) {
                    return nodeRight;
                } else if (nodeRight == null) {
                    return nodeLeft;
                } else {
                    node.rightLock.lock();
                    nodeRight.selfLock.lock();
                    try {
                        var successor = min(node.right);
                        node.key = successor.key;
                        node.value = successor.value;

                        var successorSuccessor = delete(node.key, node.right);
                        nodeRight.key = successorSuccessor.key;
                        nodeRight.value = successorSuccessor.value;

                        return node;
                    } finally {
                        node.rightLock.unlock();
                        nodeRight.selfLock.unlock();
                    }
                }
            }
        } finally {
            node.selfLock.unlock();
        }

        assert toVisit != null;
        assert toUnlock != null;

        try {
            var nn = delete(key, toVisit);
            if (cmp < 0) {
                node.left = nn;
            } else {
                node.right = nn;
            }
            return node;
        } finally {
            toUnlock.unlock();
        }
    }

    private Node findInsertionParent(K key, Node node, Node parent) {
        if (node == null) {
            if (parent != null) {
                parent.selfLock.lock();
            }
            return parent;
        }

        node.selfLock.lock();

        Node toVisit;
        Lock toUnlock;

        try {
            if (key.compareTo(node.key) < 0) {
                toVisit = node.left;
                (toUnlock = node.leftLock).lock();
            } else {
                toVisit = node.right;
                (toUnlock = node.rightLock).lock();
            }
        } finally {
            node.selfLock.unlock();
        }

        assert toVisit != null;
        assert toUnlock != null;

        try {
            return findInsertionParent(key, toVisit, node);
        } finally {
            toUnlock.unlock();
        }
    }


    private class Node {
        private volatile K key;
        private volatile V value;
        private volatile Node left, right;
        private final Lock selfLock, leftLock, rightLock;

        public Node(K key, V value) {
            this.key = key;
            this.value = value;
            this.left = null;
            this.right = null;
            this.selfLock = lockSupplier.get();
            this.leftLock = lockSupplier.get();
            this.rightLock = lockSupplier.get();
        }
    }
}
