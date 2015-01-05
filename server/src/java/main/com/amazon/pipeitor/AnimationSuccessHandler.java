package com.amazon.pipeitor;

import org.slf4j.Logger;

import java.nio.ByteBuffer;

import static org.slf4j.LoggerFactory.getLogger;

public class AnimationSuccessHandler implements RadioListener {

    private static final Logger log = getLogger(AnimationSuccessHandler.class);
    private final AnimationController animationsController;

    public AnimationSuccessHandler(AnimationController animationsController) {
        this.animationsController = animationsController;
    }

    @Override
    public void handleDataPacket(Radio radio, RemoteAddress remoteAddress, byte[] data) {
        if(data[0] == Packet.ANIMATION_SUCCESS.fingeprint) {
            log.debug("got animation success from {}", remoteAddress);
            animationsController.fireAnimationSuccess(remoteAddress);
        }
    }
}
