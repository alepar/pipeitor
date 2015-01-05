package com.amazon.pipeitor;

import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
import org.slf4j.Logger;

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
    public void handleTxStatusPacket(XBeeRadio radio, ZNetTxStatusResponse.DeliveryStatus status, byte frameId) {
        for (Map.Entry<RemoteAddress, Transmission> entry : animationsController.getTransmissions().entrySet()) {
            final RemoteAddress dst = entry.getKey();
            final Transmission transmission = entry.getValue();
            if (transmission != null && transmission.data.remaining()>0 && transmission.frameId == frameId) {
                log.debug("sending next animation data packet to {}", dst);
                AnimationResponseHandler.sendNextAnimationPacket(radio, transmission);
            }
        }
    }
}
