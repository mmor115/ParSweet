package edu.lsu.cct.parallelsuite.sets;

@FunctionalInterface
public interface FindCallback<N> {
    boolean onFound(N predecessor, N current);
}
