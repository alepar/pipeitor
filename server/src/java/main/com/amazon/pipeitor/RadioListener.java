package com.amazon.pipeitor;

import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;

public interface RadioListener {
    void handleDataPacket(Radio radio, RemoteAddress remoteAddress, byte[] data);
    void handleTxStatusPacket(XBeeRadio radio, ZNetTxStatusResponse.DeliveryStatus status, byte frameId);
}
