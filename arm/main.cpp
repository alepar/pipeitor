#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "gammas.h"

#define PIN         2
#define NUMPIXELS	7
#define GAMMA 		2.2

#define PERIOD 		300

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int blend(int t, int src, int dst) {
	if(src == dst) {
		return src;
	}

	double srcMult = gammas[1000-t];
	double dstMult = gammas[t];

	int srcR = src >> 16;
	int dstR = dst >> 16;
	int srcG = (src >> 8) & 0xff;
	int dstG = (dst >> 8) & 0xff;
	int srcB = src & 0xff;
	int dstB = dst & 0xff;

	int resR = srcR==dstR ? srcR : srcR*srcMult + dstR*dstMult;
	int resG = srcG==dstG ? srcG : srcG*srcMult + dstG*dstMult;
	int resB = srcB==dstB ? srcB : srcB*srcMult + dstB*dstMult;

	return resR<<16 | resG<<8 | resB;
}

#define FULL 	0x002000
#define EMPTY 	0x001000

#define SLOW	300
#define MED		210
#define FAST	120
#define PAUSE	200

const int animation[][NUMPIXELS+1] = {
		{ FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, SLOW	},
		{ FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, MED	},

		{ FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, SLOW	},
		{ FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, SLOW	},
		{ FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FAST	},

		{ FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, SLOW	},
		{ FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, SLOW	},
		{ FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FAST	},

		{ FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, SLOW	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, SLOW	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, MED	},

		{ FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, SLOW	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, SLOW	},

		{ FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL	, FULL 	, MED	},
		{ FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL	, FULL 	, FAST	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, FULL 	, MED	},
		{ FULL	, FULL	, FULL	, FULL	, FULL	, FULL	, EMPTY	, SLOW	},
};
const int frameCount = sizeof(animation) / sizeof(animation[0]);


int lastReport = 0;
int frameCounter = 0;

int curFrame;
int frameTimeLeft;
int lastMillis;
int curFrameTime;

void setup() {
	Serial.begin(9600);
	Serial3.begin(115200);
	pixels.begin();
	Serial.println("setup done");

	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	lastMillis = millis();
	curFrame = 0;
	curFrameTime = frameTimeLeft = animation[curFrame][NUMPIXELS];
}

void loop() {
	int curMillis = millis();
	int deltaMillis = curMillis - lastMillis;
	int nextFrame = (curFrame+1) % frameCount;

	frameTimeLeft -= deltaMillis;


	while(frameTimeLeft < 0) {
		curFrame = nextFrame;
		curFrameTime = animation[curFrame][NUMPIXELS];
		frameTimeLeft += curFrameTime;
	}

	int t = gammasLength * (curFrameTime-frameTimeLeft) / curFrameTime;

	for(int i=0; i<NUMPIXELS; i++) {
		pixels.setPixelColor(i, blend(t, animation[curFrame][i], animation[nextFrame][i]));
	}
	pixels.show();

	frameCounter++;
	int curReport = millis() / 1000;
	if(curReport != lastReport) {
		Serial.print(frameCounter);

		Serial.println();

		lastReport = curReport;
		frameCounter = 0;
	}

	lastMillis = curMillis;
}

