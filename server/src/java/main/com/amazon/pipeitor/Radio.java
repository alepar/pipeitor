package com.amazon.pipeitor;

public interface Radio {
    void addListener(RadioListener listener);

    void sendPacket(RemoteAddress dstAddress, byte[] data, byte frameId);
}
