package com.amazon.pipeitor;

public interface Radio {
    void addListener(RadioListener listener);

    void sendPacket(int[] dstAddress, byte[] data);
}
