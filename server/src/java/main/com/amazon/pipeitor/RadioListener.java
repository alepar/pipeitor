package com.amazon.pipeitor;

public interface RadioListener {
    void handleDataPacket(Radio radio, int[] remoteAddress, byte[] data);
}
