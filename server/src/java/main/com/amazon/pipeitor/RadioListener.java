package com.amazon.pipeitor;

public interface RadioListener {
    void handleDataPacket(Radio radio, RemoteAddress remoteAddress, byte[] data);
}
