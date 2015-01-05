package com.amazon.pipeitor;

import java.util.Arrays;

public class XbeeRemoteAddress implements RemoteAddress {

    private final int[] address;

    public XbeeRemoteAddress(int[] address) {
        this.address = address;
    }

    @Override
    public int[] array() {
        return address;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        final XbeeRemoteAddress that = (XbeeRemoteAddress) o;

        return Arrays.equals(address, that.address);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(address);
    }

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder();
        for (int i = 0; i < address.length; i++) {
            sb.append(String.format("%02x", address[i]));
            if(i%4 == 3) {
                sb.append(' ');
            }
        }
        if(sb.charAt(sb.length()-1) == ' ') {
            sb.setLength(sb.length()-1);
        }
        return sb.toString().toUpperCase();
    }
}
