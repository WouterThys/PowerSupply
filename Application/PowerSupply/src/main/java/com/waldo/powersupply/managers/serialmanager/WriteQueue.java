package com.waldo.powersupply.managers.serialmanager;

import java.util.LinkedList;
import java.util.Queue;

public class WriteQueue<T> {

    private final Queue<T> queue = new LinkedList<>();
    private final int capacity;
    private volatile boolean stopped = false;

    public WriteQueue(int capacity) {
        this.capacity = capacity;
    }

    public synchronized void put(T element) throws InterruptedException {
        while (size() >= capacity) {
            wait();
        }

        if (!stopped) {
            queue.add(element);
            notifyAll();
        }
    }

    public synchronized T take() throws InterruptedException {
        while (queue.isEmpty()) {
            wait();
        }
        if (!stopped) {
            T item = queue.remove();
            notifyAll();
            return item;
        }
        return null;
    }

    public synchronized void stop() {
        stopped = true;
        notifyAll();
    }

    public synchronized int size() {
        return queue.size();
    }
}

