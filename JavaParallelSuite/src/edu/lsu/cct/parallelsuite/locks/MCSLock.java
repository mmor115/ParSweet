package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

public class MCSLock implements SlimLock {
    private final ThreadLocal<QNode> myNode = ThreadLocal.withInitial(QNode::new);
    private final AtomicReference<QNode> tail = new AtomicReference<>(null);

    @Override
    public void lock() {
        var qNode = myNode.get();
        var predecessor = tail.getAndSet(qNode);

        if (predecessor != null) {
            qNode.locked.set(true);
            predecessor.next = qNode;
            while (qNode.locked.get()) {
                Thread.yield();
            }
        }
    }

    @Override
    public void unlock() {
        var qNode = myNode.get();
        if (qNode.next == null) {
            if (tail.compareAndSet(qNode, null)) {
                return;
            }
            while (qNode.next == null) {
                Thread.yield();
            }
        }

        qNode.next.locked.set(false);
        qNode.next = null;
    }

    private static class QNode {
        public final AtomicBoolean locked = new AtomicBoolean(false);
        public volatile QNode next = null;
    }
}
