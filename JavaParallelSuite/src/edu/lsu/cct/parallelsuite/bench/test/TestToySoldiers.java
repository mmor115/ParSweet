package edu.lsu.cct.parallelsuite.bench.test;

import edu.lsu.cct.parallelsuite.ToySoldiersSim;
import edu.lsu.cct.parallelsuite.bench.Misc;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class TestToySoldiers {

    public static ExecutorService pool = Executors.newCachedThreadPool(Misc.getDaemonThreadFactory());

    public static void main(String[] args) {
        var ts = new ToySoldiersSim(5, 5);
        var s1 = ts.addSoldier(2, 2);
        var s2 = ts.addSoldier(4, 4);
        ts.deploy(pool, s1, s2);

        System.out.println("Done");
    }
}