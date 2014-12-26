package com.amazon.pipeitor;

public enum Packet {

    CHECKIN((byte) 0x01),
    CHECKIN_RESPONSE((byte) 0x02);

    public final byte fingeprint;

    Packet(byte fingeprint) {
        this.fingeprint = fingeprint;
    }
}
