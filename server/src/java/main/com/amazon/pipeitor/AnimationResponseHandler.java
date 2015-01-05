package com.amazon.pipeitor;

import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
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

            final ByteBuffer animation = animationsController.getAnimation(remoteAddress);
            if(animation != null && animation.remaining()>0) {
                log.debug("got animation data for {}", remoteAddress);
                sendNextAnimationPacket(radio, remoteAddress, animation);
            }
        }
    }

    @Override
    public void handleTxStatusPacket(XBeeRadio radio, ZNetTxStatusResponse.DeliveryStatus status, int frameId) {

    }

    public static void sendNextAnimationPacket(Radio radio, RemoteAddress remoteAddress, ByteBuffer animation) {
        final int size = Math.min(animation.remaining(), 50);
        final byte[] packetData = new byte[size+1];
        packetData[0] = Packet.ANIMATION_DATA.fingeprint;
        animation.get(packetData, 1, size);
        radio.sendPacket(remoteAddress, packetData);
    }
}
