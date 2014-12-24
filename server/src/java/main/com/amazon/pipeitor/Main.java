package com.amazon.pipeitor;

import com.rapplogic.xbee.api.ApiId;
import com.rapplogic.xbee.api.XBee;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;

public class Main {

    public static void main(String[] args) throws Exception {
        final XBee xbee = new XBee();
        xbee.open("COM6", 115200);

        while (true) {
            final XBeeResponse response = xbee.getResponse();
            if (response.getApiId() == ApiId.ZNET_RX_RESPONSE) {
                final ZNetRxResponse rxResponse = (ZNetRxResponse) response;
                System.out.println("got data: " + toString(rxResponse.getData()));
            } else {
                System.out.println(response.getApiId().toString());
            }
        }
    }

    private static String toString(int[] data) {
        final char[] chars = new char[data.length];
        for (int i = 0; i < data.length; i++) {
            chars[i] = (char) data[i];
        }
        return new String(chars);
    }

}
