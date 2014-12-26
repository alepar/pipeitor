#include <stdint.h>

// --- timekeeping ----------
uint64_t curMillis;

// --- animation ------------
#define PIXELSPIN	2
#define INFO_LED	13
#define PIXELSNUM	7
#define FRAMELENGTH	(PIXELSNUM+1)

#include <Adafruit_NeoPixel.h>
#include "gammas.h"
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELSNUM, PIXELSPIN, NEO_GRB + NEO_KHZ800);

uint32_t *frames = NULL;
uint16_t frameCount = 0;
uint8_t animationState = 0;

// animation - current frame state
uint16_t curFrame;
uint32_t frameTimeLeft;
uint32_t curFrameTime;

uint64_t lastMillis;

// --- xbee state -----------
#define XBEESTATE_WAIT		1
#define XBEESTATE_RECEIVE	2
#define ADDR_BCAST_MSB 0x00000000
#define ADDR_BCAST_LSB 0x0000FFFF
#define COORDINATOR_LOST_MILLIS 		60000
#define COORDINATOR_HEARTBEAT_MILLIS 	15000
#define COORDINATOR_RETRY_MILLIS 		2500

#include <XBee.h>
XBee xbee = XBee();

uint8_t xbeeState = XBEESTATE_WAIT;

XBeeAddress64 coordinatorAddr = XBeeAddress64(ADDR_BCAST_MSB, ADDR_BCAST_LSB);
uint8_t checkinPayload[] = "checkin"; // todo construct proper checkin payload
ZBTxRequest checkinPacket = ZBTxRequest(coordinatorAddr, checkinPayload, sizeof(checkinPayload));

uint64_t checkinLastTxMillis = 0;
uint64_t checkinLastRxMillis = 0;
uint64_t animationLastRxMillis = 0;

uint32_t *data = NULL;

// --- fps counter ----------
uint64_t lastReport = 0;
uint16_t frameCounter = 0;

void printFps() {
	uint64_t curReport = curMillis / 1000;
	if(curReport != lastReport) {
		//todo uncomment
		//Serial.print(frameCounter);
		//Serial.print('\r');

		lastReport = curReport;
		frameCounter = 0;
	}

	lastMillis = curMillis;
}

void setAllPixelsTo(uint32_t color) {
	for(int i=0; i<PIXELSNUM; i++) {
		pixels.setPixelColor(i, color);
	}
	pixels.show();
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

void handleRx(ZBRxResponse& packet) {
	if (xbeeState == XBEESTATE_WAIT) {
		//todo handle incoming animation

		// todo better recognition of incoming packet type
		if(packet.getData()[0] == 'c') {
			// todo signature check
			if(coordinatorAddr.getLsb() == ADDR_BCAST_LSB && coordinatorAddr.getMsb() == ADDR_BCAST_MSB) {
				coordinatorAddr.setMsb(packet.getRemoteAddress64().getMsb());
				coordinatorAddr.setLsb(packet.getRemoteAddress64().getLsb());
				Serial.print("INFO\tnew coordinator address "); Serial.print(coordinatorAddr.getMsb(), HEX); Serial.print(" "); Serial.println(coordinatorAddr.getLsb(), HEX);
			}
			if(packet.getRemoteAddress64().getMsb() == coordinatorAddr.getMsb() && packet.getRemoteAddress64().getLsb() == coordinatorAddr.getLsb()) {
				Serial.println("INFO\tgot checkin response");
				checkinLastRxMillis = curMillis;
			}
		}

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

		animationLastRxMillis = curMillis; // todo if(successful)

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

void sendCheckin() {
	Serial.println("INFO\tsending checkin");
	digitalWrite(INFO_LED, HIGH);
  	xbee.send(checkinPacket);
  	checkinLastTxMillis = curMillis;
	digitalWrite(INFO_LED, LOW);
}

void setup() {
	Serial.begin(115200);
	Serial3.begin(115200);
	xbee.setSerial(Serial3);

	pixels.begin();
	pinMode(INFO_LED, OUTPUT);

	setAllPixelsTo(0x040000);
	delay(50);
	setAllPixelsTo(0x000000);
	Serial.println("INFO\tbooted");
	delay(500);

	waitForModem();
	blinkInfoLed(2);
	Serial.println("INFO\tmodem ATAP OK");
	delay(500);

	curMillis = millis();
	// trigger checkin at boot
	checkinLastRxMillis = checkinLastTxMillis = curMillis - COORDINATOR_HEARTBEAT_MILLIS - 1000;

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
	xbee.readPacket();
	XBeeResponse& packet = xbee.getResponse();
	if (xbee.getResponse().isAvailable()) {
		Serial.print("DEBUG\tgot packet, api id "); Serial.println(packet.getApiId(), HEX);
		ZBRxResponse rxPacket = ZBRxResponse();
		switch(packet.getApiId()) {
			case ZB_RX_RESPONSE:
				digitalWrite(INFO_LED, HIGH);
				packet.getZBRxResponse(rxPacket);
				handleRx(rxPacket);
				digitalWrite(INFO_LED, LOW);
				break;
			case ZB_TX_STATUS_RESPONSE:
				// ignore this
				// assuming that network layer is good enough for handling most common cases
				// and the rest - server will just retry
				break;
			default:
				Serial.print("WARN\tunhandled packet, api id "); Serial.println(packet.getApiId(), HEX);
		}
	}

	if (xbeeState == XBEESTATE_WAIT) {
		if ((curMillis - checkinLastRxMillis > COORDINATOR_LOST_MILLIS) &&
				(coordinatorAddr.getMsb() != ADDR_BCAST_MSB || coordinatorAddr.getLsb() != ADDR_BCAST_LSB)) {
			Serial.println("WARN\tlost coordinator, resorting to broadcast");
			coordinatorAddr.setMsb(ADDR_BCAST_MSB);
			coordinatorAddr.setLsb(ADDR_BCAST_LSB);
		}
		if ((curMillis - checkinLastRxMillis > COORDINATOR_HEARTBEAT_MILLIS) && (curMillis-checkinLastTxMillis > COORDINATOR_RETRY_MILLIS)) {
			sendCheckin();
			checkinLastTxMillis = curMillis;
		}
	}
	if (xbeeState == XBEESTATE_RECEIVE && (curMillis-animationLastRxMillis)>15000) {
		Serial.println("WARN\tdata receive timed outed");
		xbeeState = XBEESTATE_WAIT;
	}
}

