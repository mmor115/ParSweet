package edu.lsu.cct.parallelsuite.sets;

import java.util.Collection;
import java.util.Iterator;
import java.util.Set;

public interface SlimSet<E> extends Set<E> {
    @Override
    default int size() {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default boolean isEmpty() {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default Iterator<E> iterator() {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default Object[] toArray() {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default <T> T[] toArray(T[] a) {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default boolean containsAll(Collection<?> c) {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default boolean addAll(Collection<? extends E> c) {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default boolean retainAll(Collection<?> c) {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default boolean removeAll(Collection<?> c) {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    @Override
    default void clear() {
        throw new IllegalStateException("Not implemented in SlimSet.");
    }

    E getEqual(E e);
}
