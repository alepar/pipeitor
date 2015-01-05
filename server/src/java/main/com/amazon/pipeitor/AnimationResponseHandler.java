package com.amazon.pipeitor;

import org.slf4j.Logger;

import java.nio.ByteBuffer;

import static org.slf4j.LoggerFactory.getLogger;

public class AnimationResponseHandler implements RadioListener {

    private static final Logger log = getLogger(AnimationResponseHandler.class);

    private final AnimationController animationsController;

    public AnimationResponseHandler(AnimationController animationsController) {
        this.animationsController = animationsController;
    }

    @Override
    public void handleDataPacket(Radio radio, RemoteAddress remoteAddress, byte[] data) {
        if(data[0] == Packet.ANIMATION_RESPONSE.fingeprint) {
            log.debug("got animation response from {}", remoteAddress);

            final byte[] animation = animationsController.getAnimation(remoteAddress);
            if(animation != null) {
                log.debug("got animation data for {}", remoteAddress);
                final byte[] sendData = new byte[animation.length + 1];
                final ByteBuffer buf = ByteBuffer.wrap(sendData);
                buf.put(Packet.ANIMATION_DATA.fingeprint);
                buf.put(animation);

                radio.sendPacket(remoteAddress, sendData);
            }
        }
    }
}
