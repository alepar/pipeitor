package com.amazon.pipeitor;

import java.nio.ByteBuffer;

public class Transmission {

    public final byte frameId;
    public final ByteBuffer data;
    public final RemoteAddress dst;

    public Transmission(byte frameId, ByteBuffer data, RemoteAddress dst) {
        this.frameId = frameId;
        this.data = data;
        this.dst = dst;
    }

}
