package com.amazon.pipeitor;

public class GreenBuildAnimation {
    private static final int FULL =     0x002000;
    private static final int EMPTY =    0x000200;
    private static final int SLOW =     300;
    private static final int MED =      210;
    private static final int FAST =     120;
    static byte[] animation = toByteArray(
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
