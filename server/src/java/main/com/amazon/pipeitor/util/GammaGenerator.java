package com.amazon.pipeitor.util;

public class GammaGenerator {

    public static final double GAMMA = 2.2;

    public static void main(String[] args) {
        for(int i=0; i<=1000; i++) {
            System.out.print(String.format("%.5f, ", Math.pow(i / 1000.0, GAMMA)));
            if (i%10 == 9) {
                System.out.println();
            }
        }
    }

}
