package com.amazon.pipeitor;

import java.nio.ByteBuffer;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicLong;

public class RadioAnimationController implements AnimationController {

    private final ConcurrentMap<RemoteAddress, Transmission> ongoingTransmissions = new ConcurrentHashMap<>();
    private final AtomicLong frameCounter = new AtomicLong();

    private final Radio radio;

    public RadioAnimationController(Radio radio) {
        this.radio = radio;
    }

    @Override
    public Transmission getAnimation(RemoteAddress dst) {
        return ongoingTransmissions.get(dst);
    }

    @Override
    public void fireAnimationSuccess(RemoteAddress src) {
        ongoingTransmissions.remove(src);
    }

    @Override
    public void sendAnimation(RemoteAddress dst, byte[] animation) {
        radio.sendPacket(dst, new byte[] {
                Packet.ANIMATION_REQUEST.fingeprint,
                (byte)((animation.length & 0xff00) >>> 8), (byte)(animation.length & 0xff)
        }, (byte)0);

        final ByteBuffer buf = ByteBuffer.wrap(animation);
        buf.put(animation);
        buf.flip();

        ongoingTransmissions.put(dst, new Transmission(
                (byte)(frameCounter.getAndIncrement() & 0xff), buf, dst
        ));
    }

    @Override
    public Map<RemoteAddress, Transmission> getTransmissions() {
        return ongoingTransmissions;
    }

}
