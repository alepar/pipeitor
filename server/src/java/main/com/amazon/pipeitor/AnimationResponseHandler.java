package com.amazon.pipeitor;

import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
import org.slf4j.Logger;

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

            final Transmission transmission = animationsController.getAnimation(remoteAddress);
            if(transmission != null && transmission.getCurPacket() != null) {
                log.debug("got animation data for {}", remoteAddress);
                radio.sendPacket(transmission.dst, transmission.getCurPacket(), transmission.frameId);
                transmission.updateLastSend();
            }
        }
    }

    @Override
    public void handleTxStatusPacket(XBeeRadio radio, ZNetTxStatusResponse.DeliveryStatus status, byte frameId) {

    }

}
