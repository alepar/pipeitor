package com.amazon.pipeitor;

import org.slf4j.Logger;

import static org.slf4j.LoggerFactory.getLogger;

public class Transmission {

    private static final int PACKET_SIZE = 56;

    private static final Logger log = getLogger(Transmission.class);

    // todo this is ugly - go generic?
    public final RemoteAddress dst;
    public final byte frameId;

    private final byte[] data;
    private final byte fingerprint;

    private int curPacket = 0;
    private long lastSend;

    public Transmission(RemoteAddress dst, byte frameId, byte[] data, byte fingerprint) {
        this.frameId = frameId;
        this.data = data;
        this.dst = dst;
        this.fingerprint = fingerprint;
    }

    public void acknowledgeLastPacket() {
        curPacket++;
    }

    public byte[] getCurPacket() {
        final int curOffset = curPacket * (PACKET_SIZE - 1);
        if(curOffset >= data.length) {
            return null;
        }

        final byte[] packet = new byte[Math.min(PACKET_SIZE, data.length-curOffset+1)];
        packet[0] = fingerprint;
        System.arraycopy(data, curOffset, packet, 1, packet.length-1);

        return packet;
    }

    public void updateLastSend() {
        lastSend = System.nanoTime();
    }

    public long timeSinceLastSend() {
        return System.nanoTime() - lastSend;
    }

}
