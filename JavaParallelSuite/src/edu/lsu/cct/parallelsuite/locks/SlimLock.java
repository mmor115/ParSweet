package edu.lsu.cct.parallelsuite.locks;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;

public interface SlimLock extends Lock {
    @Override
    default void lockInterruptibly() {
        throw new IllegalStateException("lockInterruptibly() is not implemented.");
    }

    @Override
    default boolean tryLock() {
        throw new IllegalStateException("tryLock() is not implemented.");
    }

    @Override
    default boolean tryLock(long time, TimeUnit unit) {
        throw new IllegalStateException("tryLock(long time, TimeUnit unit) is not implemented.");
    }


    @Override
    default Condition newCondition() {
        throw new IllegalStateException("newCondition() is not implemented.");
    }
}
