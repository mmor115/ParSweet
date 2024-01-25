package edu.lsu.cct.parallelsuite.maps;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

public interface SlimMap<K, V> extends Map<K, V> {
    @Override
    default int size() {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default boolean isEmpty() {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default boolean containsKey(Object key) {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default boolean containsValue(Object value) {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default void putAll(Map<? extends K, ? extends V> m) {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default void clear() {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default Set<K> keySet() {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default Collection<V> values() {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }

    @Override
    default Set<Entry<K, V>> entrySet() {
        throw new IllegalStateException("Not implemented in SlimMap.");
    }
}
