package com.amazon.pipeitor;

import com.rapplogic.xbee.api.XBee;

public class Main {

    public static void main(String[] args) throws Exception {
        final XBee xbee = new XBee();
        xbee.open("COM6", 115200);
        final Radio radio = new XBeeRadio(xbee);
        radio.addListener(new CheckinHandler());
    }

}
