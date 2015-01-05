package com.amazon.pipeitor;

import com.rapplogic.xbee.api.XBee;

public class Main {

    public static void main(String[] args) throws Exception {
        final XBee xbee = new XBee();
        xbee.open("COM6", 19200);
        final Radio radio = new XBeeRadio(xbee);

        final AnimationController animationsController = new RadioAnimationController(radio);

        radio.addListener(new CheckinHandler(animationsController));
        radio.addListener(new AnimationResponseHandler(animationsController));
        radio.addListener(new AnimationSuccessHandler(animationsController));

        while(true) {
            Thread.sleep(10000);
            animationsController.sendAnimation(
                    new XbeeRemoteAddress(new int[] {0x00, 0x13, 0xA2, 0x00, 0x40, 0xA9, 0x7E, 0x57}),
                    toByteArray(
                            0x800000, 0x800000, 0x800000, 0x800000, 0x800000, 0x800000, 0x800000, 0xff,
                            0x400000, 0x400000, 0x400000, 0x400000, 0x400000, 0x400000, 0x400000, 0xff
                    )
            );
        }
    }

    private static byte[] toByteArray(int... data) {
        int j=0;
        final byte[] bytes = new byte[data.length*3];
        for (int i = 0; i < data.length; i++) {
            bytes[j++] = (byte)((data[i] & 0xff0000) >>> 16);
            bytes[j++] = (byte)((data[i] & 0xff00) >>> 8);
            bytes[j++] = (byte)(data[i] & 0xff);
        }
        return bytes;
    }

}
