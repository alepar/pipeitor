package com.amazon.pipeitor;

import java.nio.ByteBuffer;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class RadioAnimationController implements AnimationController {

    private final ConcurrentMap<RemoteAddress, ByteBuffer> animationsToSend = new ConcurrentHashMap<>();

    private final Radio radio;

    public RadioAnimationController(Radio radio) {
        this.radio = radio;
    }

    @Override
    public ByteBuffer getAnimation(RemoteAddress dst) {
        return animationsToSend.get(dst);
    }

    @Override
    public void fireAnimationSuccess(RemoteAddress src) {
        animationsToSend.remove(src);
    }

    @Override
    public void sendAnimation(RemoteAddress dst, byte[] animation) {
        radio.sendPacket(dst, new byte[] {
                Packet.ANIMATION_REQUEST.fingeprint,
                (byte)((animation.length & 0xff00) >>> 8), (byte)(animation.length & 0xff)
        });

        final ByteBuffer buf = ByteBuffer.wrap(animation);
        buf.put(animation);
        buf.flip();

        animationsToSend.put(dst, buf);
    }

    @Override
    public Map<RemoteAddress, ByteBuffer> getAnimations() {
        return animationsToSend;
    }
}
