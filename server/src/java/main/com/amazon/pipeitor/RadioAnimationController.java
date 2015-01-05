package com.amazon.pipeitor;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class RadioAnimationController implements AnimationController {

    private final ConcurrentMap<RemoteAddress, byte[]> animationsToSend = new ConcurrentHashMap<>();

    private final Radio radio;

    public RadioAnimationController(Radio radio) {
        this.radio = radio;
    }

    @Override
    public byte[] getAnimation(RemoteAddress dst) {
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

        animationsToSend.put(dst, animation);
    }
}
