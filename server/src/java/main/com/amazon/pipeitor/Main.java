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
        radio.addListener(new TransmissionHandler(animationsController));

        while(true) {
            Thread.sleep(10000);
        }
    }

}
