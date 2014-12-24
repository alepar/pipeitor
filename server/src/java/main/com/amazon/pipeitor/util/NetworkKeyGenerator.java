package com.amazon.pipeitor.util;

import java.math.BigInteger;
import java.security.SecureRandom;

public class NetworkKeyGenerator {

    public static void main(String[] args) {
        final byte[] bytes = new byte[16];
        new SecureRandom().nextBytes(bytes);
        System.out.println(new BigInteger(bytes).toString(16));
    }

}
