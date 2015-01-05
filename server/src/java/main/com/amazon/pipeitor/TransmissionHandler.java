package com.amazon.pipeitor;

import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
import org.slf4j.Logger;

import java.nio.ByteBuffer;
import java.util.Map;

import static org.slf4j.LoggerFactory.getLogger;

public class TransmissionHandler implements RadioListener {

    private static final Logger log = getLogger(TransmissionHandler.class);

    private final AnimationController animationsController;

    public TransmissionHandler(AnimationController animationsController) {
        this.animationsController = animationsController;
    }

    @Override
    public void handleDataPacket(Radio radio, RemoteAddress remoteAddress, byte[] data) {

    }

    @Override
    public void handleTxStatusPacket(XBeeRadio radio, ZNetTxStatusResponse.DeliveryStatus status, int frameId) {
        for (Map.Entry<RemoteAddress, ByteBuffer> entry : animationsController.getAnimations().entrySet()) {
            final RemoteAddress dst = entry.getKey();
            final ByteBuffer animation = entry.getValue();
            if (animation != null && animation.remaining()>0) {
                log.debug("sending next animation data packet to {}", dst);
                AnimationResponseHandler.sendNextAnimationPacket(radio, dst, animation);
            }
        }
    }
}
