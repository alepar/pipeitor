package com.amazon.pipeitor;

import org.slf4j.Logger;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicLong;

import static org.slf4j.LoggerFactory.getLogger;

public class RadioAnimationController implements AnimationController {

    private static final Logger log = getLogger(RadioAnimationController.class);

    private static final long TIMEOUT_NANOS = 1_000_000_000;

    private final ConcurrentMap<RemoteAddress, Transmission> ongoingTransmissions = new ConcurrentHashMap<>();
    private final AtomicLong frameCounter = new AtomicLong();

    private final Radio radio;

    public RadioAnimationController(Radio radio) {
        this.radio = radio;

        new TimeoutWatchdogThread().start();
    }

    @Override
    public Transmission getAnimation(RemoteAddress dst) {
        return ongoingTransmissions.get(dst);
    }

    @Override
    public void fireAnimationSuccess(RemoteAddress src) {
        ongoingTransmissions.remove(src);
    }

    @Override
    public void sendAnimation(RemoteAddress dst, byte[] animation) {
        // todo these need retry
        radio.sendPacket(
                dst,
                new byte[] {
                    Packet.ANIMATION_REQUEST.fingeprint,
                    (byte)((animation.length & 0xff00) >>> 8), (byte)(animation.length & 0xff)
                },
                (byte)0
        );

        final Transmission transmission = new Transmission(
                dst,
                (byte) (frameCounter.incrementAndGet() & 0xff),
                animation, Packet.ANIMATION_DATA.fingeprint
        );
        transmission.updateLastSend();
        ongoingTransmissions.put(dst, transmission);
    }

    @Override
    public Map<RemoteAddress, Transmission> getTransmissions() {
        return ongoingTransmissions;
    }

    private class TimeoutWatchdogThread extends Thread {

        public TimeoutWatchdogThread() {
            setName("RadioAnimationController-TimeoutWatchdog" + getName());
            setDaemon(true);
        }

        @Override
        public void run() {
            try {
                while(!Thread.interrupted()) {
                    for (Transmission transmission : ongoingTransmissions.values()) {
                        final byte[] packet = transmission.getCurPacket();
                        if (packet != null && transmission.timeSinceLastSend() > TIMEOUT_NANOS) {
                            log.debug("resending timeouted animation data packet to {}", transmission.dst);
                            radio.sendPacket(transmission.dst, packet, transmission.frameId);
                            transmission.updateLastSend();
                        }
                    }

                    Thread.sleep(100);
                }
            } catch (InterruptedException ignored) { }

            log.warn("exiting");
        }
    }
}
