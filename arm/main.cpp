#define PIN         2
#define INFO_LED		13

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
	uint64_t curReport = curMillis / 1000;
	if(curReport != lastReport) {
		Serial.print(frameCounter);
		Serial.print('\r');

		lastReport = curReport;
		frameCounter = 0;
	}

	lastMillis = curMillis;
}

void displayFrame() {
	// todo fix animation
/*	uint8_t deltaMillis = curMillis - lastMillis;
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
	}*/
	pixels.show();

	frameCounter++;
}

void handleIncomingData(ZBRxResponse& packet) {
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
		digitalWrite(INFO_LED, HIGH);
		delay(25);
		digitalWrite(INFO_LED, LOW);
		if(i+1<count){
			delay(100);
		}
	}
}

void waitForModem() {
	uint8_t at[2] = {'A', 'P'};
	while(1) {
		AtCommandRequest cmdAt = AtCommandRequest(at);
		xbee.send(cmdAt);
		Serial.println("DEBUG\tsent modem ATAP");
		xbee.readPacket(100);
		Serial.println("DEBUG\treadPacket finished");
		if(xbee.getResponse().isAvailable()) {
			Serial.println("DEBUG\tresponse available");
			if (xbee.getResponse().getApiId() == AT_RESPONSE) {
				Serial.println("DEBUG\tgot ATAP command response");
				AtCommandResponse atResponse = AtCommandResponse();
				xbee.getResponse().getAtCommandResponse(atResponse);
				Serial.print("DEBUG\tATAP status: "); Serial.println(atResponse.getStatus());
				if (atResponse.getStatus() == 0) {
					return; // todo assert that we have ATAP2
				}
			}
		}
		delay(100);
	}
}

uint8_t checkinPayload[] = "checkin"; // todo construct proper checkin payload
XBeeAddress64 addrBcast = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest txCheckin = ZBTxRequest(addrBcast, checkinPayload, sizeof(checkinPayload));

void sendCheckin() {
	Serial.println("INFO\tsending checkin");
	digitalWrite(INFO_LED, HIGH);
  	xbee.send(txCheckin);
	digitalWrite(INFO_LED, LOW);
}

void waitForBootCheckin() {
  	while(1) {
	  	sendCheckin();
	  	uint64_t sentMillis = millis();
	  	while(millis() - sentMillis < 2500) {
		  	xbee.readPacket(100);
		  	if (xbee.getResponse().isAvailable()) {
		  		XBeeResponse& packet = xbee.getResponse();
		  		Serial.print("DEBUG\tgot packet "); Serial.println(packet.getApiId());
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

	pinMode(INFO_LED, OUTPUT);
	blinkInfoLed(1);
	Serial.println("INFO\tbooted");
	delay(500);

	waitForModem();
	blinkInfoLed(2);
	Serial.println("INFO\tmodem AT OK");
	delay(500);

	waitForBootCheckin();
	blinkInfoLed(3);
	Serial.println("INFO\tboot checkin passed");
	delay(500);

	// TODO move chunk below to 'start animation'
	/*lastMillis = millis();
	curFrame = 0;
	curFrameTime = frameTimeLeft = animation[curFrame*FRAMELENGTH + NUMPIXELS];*/
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
		xbeeLastTransmissionMillis = curMillis;
	}
	if (xbeeState == XBEESTATE_RECEIVE && (curMillis-xbeeLastTransmissionMillis)>15000) {
		Serial.println("WARN\tdata receive timed outed");
		xbeeState = XBEESTATE_WAIT;
	}

	xbee.readPacket();
	XBeeResponse& packet = xbee.getResponse();
	if (xbee.getResponse().isAvailable()) {
		Serial.print("DEBUG\tgot packet, api id="); Serial.println(packet.getApiId());
		ZBRxResponse rxPacket = ZBRxResponse();
		switch(packet.getApiId()) {
			case ZB_RX_RESPONSE:
				digitalWrite(INFO_LED, HIGH);
				packet.getZBRxResponse(rxPacket);
				handleIncomingData(rxPacket);
				digitalWrite(INFO_LED, LOW);
				break;
			case ZB_TX_STATUS_RESPONSE:
				// ignore this
				// assuming that network layer is good enough for handling most common cases
				// and the rest - server will just retry
				break;
			default:
				Serial.print("WARN\tunhandled packet, api id=");
				Serial.println(packet.getApiId());
		}
	}
}

