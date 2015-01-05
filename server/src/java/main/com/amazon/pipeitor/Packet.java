package com.amazon.pipeitor;

public enum Packet {

    CHECKIN((byte) 0x01),
    CHECKIN_RESPONSE((byte) 0x02),
    ANIMATION_REQUEST((byte) 0x03),
    ANIMATION_RESPONSE((byte) 0x04),
    ANIMATION_DATA((byte) 0x05),
    ANIMATION_SUCCESS((byte) 0x06);

    public final byte fingeprint;

    Packet(byte fingeprint) {
        this.fingeprint = fingeprint;
    }
}
