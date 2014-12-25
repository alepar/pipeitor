#define PIN         2
#define INFOLED		13

#define NUMPIXELS	7
#define FRAMELENGTH	(NUMPIXELS+1)

#define XBEESTATE_WAIT		1
#define XBEESTATE_RECEIVE	2

#include <math.h>

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#include <XBee.h>
XBee xbee = XBee();

#include "gammas.h"


// animation
uint32_t *frames = NULL;
uint16_t frameCount = 0;
uint8_t animationState = 0;

// animation - current frame state
uint16_t curFrame;
uint32_t frameTimeLeft;
uint32_t curFrameTime;

uint64_t lastMillis;
uint64_t curMillis;

// xbee state
uint8_t xbeeState = XBEESTATE_WAIT;
uint64_t xbeeLastTransmissionMillis;

uint32_t *data = NULL;
uint32_t rxSH, rxSL;

// fps counter
uint64_t lastReport = 0;
uint16_t frameCounter = 0;

void printFps() {
	int curReport = curMillis / 1000;
	if(curReport != lastReport) {
		Serial.print(frameCounter);
		Serial.print('\r');

		lastReport = curReport;
		frameCounter = 0;
	}

	lastMillis = curMillis;
}

void displayFrame() {
	uint8_t deltaMillis = curMillis - lastMillis;
	uint16_t nextFrame = (curFrame+1) % frameCount;

	frameTimeLeft -= deltaMillis;

	while(frameTimeLeft < 0) {
		curFrame = nextFrame;
		nextFrame = (curFrame+1) % frameCount;

		curFrameTime = animation[curFrame*FRAMELENGTH + NUMPIXELS];
		frameTimeLeft += curFrameTime;
	}

	uint16_t t = gammasLength * (curFrameTime-frameTimeLeft) / curFrameTime;

	for(int i=0; i<NUMPIXELS; i++) {
		pixels.setPixelColor(i, blend(t, animation[curFrame*FRAMELENGTH + i], animation[nextFrame*FRAMELENGTH + i]));
	}
	pixels.show();

	frameCounter++;
}

void (ZBRxResponse& packet) {
	if (xbeeState == XBEESTATE_WAIT) {
		//todo handle checkin response or incoming animation

		/*
			incoming animation:
			* read header (length)
			* allocate memory
			* remember source address
		*/

		/*
			checkin response - update lastCheckin time if in proper state
		*/
	}
	else if(xbeeState == XBEESTATE_RECEIVE) {
		//todo handle incoming animation

		//check for sender address

		xbeeLastTransmissionMillis = curMillis; // todo if(successful)

		//check if this is all of it and send ACK
	}
}

void blinkInfoLed(uint8_t count) {
	for (uint8_t i=0; i<count; i++) {
		digitalWrite(INFOLED, HIGH);
		delay(50);
		digitalWrite(INFOLED, LOW);
		if(i+1<count){
			delay(200);
		}
	}
}

void waitForModem() {
	while(1) {
		AtCommandRequest cmdAt = AtCommandRequest("");
		xbee.send(cmdAt);
		xbee.readPacket(100);
		if(xbee.getResponse().isAvailable) {
			if (xbee.getResponse().getApiId() == AT_RESPONSE) {
				AtCommandResponse atResponse = AtCommandResponse();
				xbee.getResponse().getAtCommandResponse(atResponse);
				if (atResponse.getStatus() == 0) {
					return;
				}
			}
		}
	}
}

uint8_t checkinPayload[] = "checkin"; // todo construct proper checkin payload
XBeeAddress64 addrBcast = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest txCheckin = ZBTxRequest(addrBcast, checkinPayload, sizeof(checkinPayload));

void sendCheckin() {
	Serial.println("INFO\tsending checkin");
  	xbee.send(txCheckin);
}

void waitForBootCheckin() {
  	while(1) {
	  	sendCheckin();
	  	uint64_t sentMillis = millis();
	  	while(millis() - sentMillis < 500) {
		  	xbee.readPacket(100);
		  	if (xbee.getResponse().isAvailable()) {
		  		XBeeResponse& packet = xbee.getResponse();
		  		if(packet.getApiId() == ZB_RX_RESPONSE) {
					ZBRxResponse rxPacket;
					packet.getZBRxResponse(rxPacket);
					// todo check that checkin response belongs to coordinator

					Serial.println("INFO\tcheckin OK");
					xbeeState = XBEESTATE_WAIT;
					xbeeLastTransmissionMillis = millis();
					return;
		  		}
		  	}
	    }
	}	
}

void setup() {
	Serial.begin(115200);
	Serial3.begin(115200);
	xbee.setSerial(Serial3);

	pixels.begin();

	pinMode(INFOLED, OUTPUT);
	blinkInfoLed(1);
	Serial.println("INFO\tbooted");

	waitForModem();
	blinkInfoLed(2);
	Serial.println("INFO\tmodem AT OK");

	waitForBootCheckin();
	blinkInfoLed(3);
	Serial.println("INFO\tboot checkin passed");

	// TODO move chunk below to 'start animation'
	lastMillis = millis();
	curFrame = 0;
	curFrameTime = frameTimeLeft = animation[curFrame*FRAMELENGTH + NUMPIXELS];
}

void loop() {
	curMillis = millis();
	if(frames && frameCounter) {
		displayFrame();
	}
	printFps();	

	curMillis = millis();
	if (xbeeState == XBEESTATE_WAIT && (curMillis-xbeeLastTransmissionMillis)>15000) {
		sendCheckin();
	}
	if (xbeeState == XBEESTATE_RECEIVE && (curMillis-xbeeLastTransmissionMillis)>15000) {
		Serial.println("WARN\tdata receive timed outed");
		xbeeState = XBEESTATE_WAIT;
	}

	xbee.readPacket();
	XBeeResponse& packet = xbee.getResponse();
	if (xbee.getResponse().isAvailable()) {
		Serial.print("DEBUG\tgot packet, api id: "); Serial.println(packet.getApiId());
		switch(packet.getApiId()) {
			case ZB_RX_RESPONSE:
				digitalWrite(INFOLED, HIGH);
				ZBRxResponse rxPacket;
				packet.getZBRxResponse(rxPacket);
				handleIncomingData(rxPacket);
				digitalWrite(INFOLED, LOW);
				break;
			case ZB_TX_STATUS_RESPONSE:
				// ignore this
				// assuming that network layer is good enough for handling most common cases
				// and the rest - server will just retry
				break;
			default:
				Serial.print("WARN\tunhandled packet, api id: "); 
				Serial.println(packet.getApiId());
		}
	}
}

