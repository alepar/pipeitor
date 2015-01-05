package com.amazon.pipeitor;

public interface AnimationController {

    void sendAnimation(RemoteAddress dst, byte[] animation);
    byte[] getAnimation(RemoteAddress dst);
    void fireAnimationSuccess(RemoteAddress src);
}
