package com.amazon.pipeitor;

import com.rapplogic.xbee.api.XBee;
import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;
import com.rapplogic.xbee.api.zigbee.ZNetTxRequest;
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

        new XBeeRadioThread().start();
    }

    @Override
    public void addListener(RadioListener listener) {
        listeners.add(listener);
    }

    @Override
    public synchronized void sendPacket(int[] dstAddress, byte[] data) {
        try {
            xbee.sendRequest(new ZNetTxRequest(new XBeeAddress64(dstAddress), toIntArray(data)));
        } catch (IOException e) {
            throw new RuntimeException("failed to send packet", e);
        }
    }

    private class XBeeRadioThread extends Thread {

        public XBeeRadioThread() {
            setName("XBeeRadioThread");
        }

        @Override
        public void run() {
            while (!Thread.interrupted()) {
                try {
                    synchronized (XBeeRadio.this) {
                        final XBeeResponse response = xbee.getResponse();
                        log.debug("got packet {}", response.getApiId());
                        switch(response.getApiId()) {
                            case ZNET_RX_RESPONSE:
                                final ZNetRxResponse rxResponse = (ZNetRxResponse) response;
                                if (rxResponse.getData().length == 0) {
                                    log.warn("data is empty, skipping");
                                    continue;
                                }
                                for (RadioListener listener : listeners) {
                                    listener.handleDataPacket(XBeeRadio.this, rxResponse.getRemoteAddress64().getAddress(), toByteArray(rxResponse.getData()));
                                }
                                break;
                            case ZNET_TX_STATUS_RESPONSE:
                                //todo handle this?
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

    private static String toString(int[] data) {
        final char[] chars = new char[data.length];
        for (int i = 0; i < data.length; i++) {
            chars[i] = (char) data[i];
        }
        return new String(chars);
    }

}
