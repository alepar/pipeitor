package com.amazon.pipeitor;

import org.slf4j.Logger;

import static org.slf4j.LoggerFactory.getLogger;

public class CheckinHandler implements RadioListener {

    private static final Logger log = getLogger(CheckinHandler.class);

    @Override
    public void handleDataPacket(Radio radio, int[] remoteAddress, byte[] data) {
        if(data[0] == Packet.CHECKIN.fingeprint) {
            log.debug("got checkin from {}", addressToString(remoteAddress));
            radio.sendPacket(remoteAddress, new byte[]{Packet.CHECKIN_RESPONSE.fingeprint});
        }
    }

    private static String addressToString(int[] address) {
        final StringBuilder sb = new StringBuilder();
        for (int i = 0; i < address.length; i++) {
            sb.append(String.format("%02x", address[i]));
            if(i%4 == 3) {
                sb.append(' ');
            }
        }
        if(sb.charAt(sb.length()-1) == ' ') {
            sb.setLength(sb.length()-1);
        }
        return sb.toString().toUpperCase();
    }

}
