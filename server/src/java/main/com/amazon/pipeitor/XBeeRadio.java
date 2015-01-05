package com.amazon.pipeitor;

import com.rapplogic.xbee.api.XBee;
import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;
import com.rapplogic.xbee.api.zigbee.ZNetTxRequest;
import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
import org.slf4j.Logger;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import static org.slf4j.LoggerFactory.getLogger;

public class XBeeRadio implements Radio {

    private static final Logger log = getLogger(XBeeRadio.class);

    private final XBee xbee;
    private final List<RadioListener> listeners = new CopyOnWriteArrayList<>();

    public XBeeRadio(XBee xbee) {
        this.xbee = xbee;

        xbee.addPacketListener(new PacketListener());
    }

    private class PacketListener implements com.rapplogic.xbee.api.PacketListener {
        @Override
        public void processResponse(XBeeResponse xBeeResponse) {
            try {
                synchronized (XBeeRadio.this) {
                    final XBeeResponse response = xbee.getResponse();
                    log.debug("got packet {}", response.getApiId());
                    switch(response.getApiId()) {
                        case ZNET_RX_RESPONSE:
                            final ZNetRxResponse rxResponse = (ZNetRxResponse) response;
                            if (rxResponse.getData().length == 0) {
                                log.warn("data is empty, skipping");
                                return;
                            }
                            for (RadioListener listener : listeners) {
                                listener.handleDataPacket(XBeeRadio.this, new XbeeRemoteAddress(rxResponse.getRemoteAddress64().getAddress()), toByteArray(rxResponse.getData()));
                            }
                            break;
                        case ZNET_TX_STATUS_RESPONSE:
                            final ZNetTxStatusResponse statusResponse = (ZNetTxStatusResponse) response;
                            for (RadioListener listener : listeners) {
                                listener.handleTxStatusPacket(XBeeRadio.this, statusResponse.getDeliveryStatus(), (byte) statusResponse.getFrameId());
                            }
                            break;
                        default:
                            log.warn("unknown packet {}", response.getApiId());
                    }
                }
            } catch (Exception e) {
                log.warn("uncaught exception in event handler loop", e);
            }
        }
    }

    @Override
    public void addListener(RadioListener listener) {
        listeners.add(listener);
    }

    @Override
    public synchronized void sendPacket(RemoteAddress dstAddress, byte[] data, byte frameId) {
        try {
            final ZNetTxRequest request = new ZNetTxRequest(new XBeeAddress64(dstAddress.array()), toIntArray(data));
            request.setFrameId(frameId);
            xbee.sendRequest(request);
        } catch (IOException e) {
            throw new RuntimeException("failed to send packet", e);
        }
    }

    private static int[] toIntArray(byte[] bytes) {
        final int[] ints = new int[bytes.length];
        for (int i = 0; i < bytes.length; i++) {
            ints[i] = bytes[i];
        }
        return ints;
    }

    private static byte[] toByteArray(int[] ints) {
        final byte[] bytes = new byte[ints.length];
        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = (byte) ints[i];
        }
        return bytes;
    }

}
