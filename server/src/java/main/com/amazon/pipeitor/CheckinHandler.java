package com.amazon.pipeitor;

import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
import org.slf4j.Logger;

import static org.slf4j.LoggerFactory.getLogger;

public class CheckinHandler implements RadioListener {

    private static final Logger log = getLogger(CheckinHandler.class);

    private final AnimationController animationsController;

    public CheckinHandler(AnimationController animationsController) {
        this.animationsController = animationsController;
    }

    @Override
    public void handleDataPacket(Radio radio, RemoteAddress remoteAddress, byte[] data) {
        if(data[0] == Packet.CHECKIN.fingeprint) {
            log.debug("got checkin from {}", remoteAddress);
            radio.sendPacket(remoteAddress, new byte[]{Packet.CHECKIN_RESPONSE.fingeprint}, (byte)0);
            animationsController.sendAnimation(remoteAddress, GreenBuildAnimation.animation);
        }
    }

    @Override
    public void handleTxStatusPacket(XBeeRadio radio, ZNetTxStatusResponse.DeliveryStatus status, byte frameId) {

    }
}
