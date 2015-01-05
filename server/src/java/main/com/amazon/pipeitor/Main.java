package com.amazon.pipeitor;

import com.rapplogic.xbee.api.XBee;

public class Main {

    private static final int FULL =     0x002000;
    private static final int EMPTY =    0x000500;

    private static final int SLOW =     300;
    private static final int MED =      210;
    private static final int FAST =     120;

    private static byte[] animation = toByteArray(
            FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, SLOW	,
            FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, MED   ,
            FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, FAST  ,
            FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, FAST  ,
            FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, MED   ,

            FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, SLOW	,
            FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, SLOW	,
            FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, MED	,
            FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FAST	,

            FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, MED	,
            FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, SLOW	,
            FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, SLOW	,
            FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, MED	,
            FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, MED	,
            FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FAST	,

            FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, MED	,
            FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, SLOW	,
            FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, SLOW	,
            FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, MED	,

            FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, MED	,
            FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, SLOW	,
            FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, SLOW	,

            FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, MED	,
            FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, FAST	,
            FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, MED	,
            FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, SLOW
    );

    public static void main(String[] args) throws Exception {
        final XBee xbee = new XBee();
        xbee.open("COM6", 19200);
        final Radio radio = new XBeeRadio(xbee);

        final AnimationController animationsController = new RadioAnimationController(radio);

        radio.addListener(new CheckinHandler(animationsController));
        radio.addListener(new AnimationResponseHandler(animationsController));
        radio.addListener(new AnimationSuccessHandler(animationsController));
        radio.addListener(new TransmissionHandler(animationsController));

        while(true) {
            Thread.sleep(10000);
            animationsController.sendAnimation(
                    new XbeeRemoteAddress(new int[] {0x00, 0x13, 0xA2, 0x00, 0x40, 0xA9, 0x7E, 0x57}),
                    animation
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
