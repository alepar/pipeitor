package com.amazon.pipeitor;

import java.nio.ByteBuffer;
import java.util.Map;

public interface AnimationController {
    void sendAnimation(RemoteAddress dst, byte[] animation);
    ByteBuffer getAnimation(RemoteAddress dst);
    void fireAnimationSuccess(RemoteAddress src);
    Map<RemoteAddress, ByteBuffer> getAnimations();
}
