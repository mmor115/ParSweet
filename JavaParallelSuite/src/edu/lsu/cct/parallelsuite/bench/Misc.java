package edu.lsu.cct.parallelsuite.bench;

import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;

public class Misc {
    public static ThreadFactory getDaemonThreadFactory() {
        return r -> {
            var t = Executors.defaultThreadFactory().newThread(r);
            t.setDaemon(true);
            return t;
        };
    }
}
