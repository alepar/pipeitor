package com.amazon.pipeitor;

import java.util.Map;

public interface AnimationController {
    void sendAnimation(RemoteAddress dst, byte[] animation);
    Transmission getAnimation(RemoteAddress dst);
    void fireAnimationSuccess(RemoteAddress src);
    Map<RemoteAddress, Transmission> getTransmissions();
}
