package com.amazon.pipeitor;

import org.slf4j.Logger;

import java.util.Arrays;

import static org.slf4j.LoggerFactory.getLogger;

public class CheckinHandler implements RadioListener {

    private static final Logger log = getLogger(CheckinHandler.class);

    private static byte[] checkinFingerprint = "checkin\0".getBytes();

    @Override
    public void handleDataPacket(Radio radio, int[] remoteAddress, byte[] data) {
        if(Arrays.equals(data, checkinFingerprint)) {
            log.debug("got checkin from {}", addressToString(remoteAddress));
            radio.sendPacket(remoteAddress, "checkin ok\0".getBytes());
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
