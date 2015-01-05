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
uint16_t framesCount = 0;
uint8_t animationState = 0;

// animation - current frame state
uint16_t curFrame;
int32_t frameTimeLeft;
int32_t curFrameTime;

uint64_t lastMillis;

// --- xbee state -----------
#define XBEESTATE_WAIT		1
#define XBEESTATE_RECEIVE	2
#define ADDR_BCAST_MSB 0x00000000
#define ADDR_BCAST_LSB 0x0000FFFF
#define COORDINATOR_LOST_MILLIS 		60000
#define COORDINATOR_HEARTBEAT_MILLIS 	15000
#define COORDINATOR_RETRY_MILLIS 		2500
#define DATA_TRANSFER_TIMEOUT_MILLIS	15000

#define PKT_CHECKIN 			0x01
#define PKT_CHECKIN_RESPONSE 	0x02
#define PKT_ANIMATION_REQUEST 	0x03
#define PTK_ANIMATION_RESPONSE 	0x04
#define PKT_ANIMATION_DATA 		0x05
#define PTK_ANIMATION_SUCCESS	0x06

#include <XBee.h>
XBee xbee = XBee();

uint8_t xbeeState = XBEESTATE_WAIT;

XBeeAddress64 coordinatorAddr = XBeeAddress64(ADDR_BCAST_MSB, ADDR_BCAST_LSB);

uint8_t checkinPayload[] = { PKT_CHECKIN }; // todo construct proper checkin payload
ZBTxRequest checkinPacket = ZBTxRequest(coordinatorAddr, checkinPayload, sizeof(checkinPayload));

uint8_t animationResponsePayload[] = {PTK_ANIMATION_RESPONSE};
ZBTxRequest animationResponsePacket = ZBTxRequest(coordinatorAddr, animationResponsePayload, sizeof(animationResponsePayload));

uint8_t animationSuccessPayload[] = {PTK_ANIMATION_SUCCESS};
ZBTxRequest animationSuccessPacket = ZBTxRequest(coordinatorAddr, animationSuccessPayload, sizeof(animationSuccessPayload));

uint64_t checkinLastTxMillis = 0;
uint64_t checkinLastRxMillis = 0;
uint64_t animationLastRxMillis = 0;

#define MAX_ANIMATION_LENGTH 5120
uint32_t data[2][MAX_ANIMATION_LENGTH];
uint16_t dataIncomingLength;
byte dataNextSlot = 0;
uint16_t dataTransferOffset;
uint8_t dataTransferByte;

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
}

void setAllPixelsTo(uint32_t color) {
	for(int i=0; i<PIXELSNUM; i++) {
		pixels.setPixelColor(i, color);
	}
	pixels.show();
}

void displayFrame() {
	uint64_t deltaMillis = curMillis - lastMillis;
	if(deltaMillis < 1) {
		return; //fps limiter
	}
	uint16_t nextFrame = (curFrame+1) % framesCount;

	frameTimeLeft -= deltaMillis;

	while(frameTimeLeft < 0) {
		curFrame = nextFrame;
		nextFrame = (curFrame+1) % framesCount;

		curFrameTime = frames[curFrame*FRAMELENGTH + PIXELSNUM];
		frameTimeLeft += curFrameTime;
	}

	uint32_t t = 1000 * (curFrameTime-frameTimeLeft) / curFrameTime;
	for(int i=0; i<PIXELSNUM; i++) {
		pixels.setPixelColor(i, blend(t, frames[curFrame*FRAMELENGTH + i], frames[nextFrame*FRAMELENGTH + i]));
	}
	pixels.show();

	lastMillis = curMillis;
	frameCounter++;
}

bool packetFromCoordinator(ZBRxResponse& packet) {
	return packet.getRemoteAddress64().getMsb() == coordinatorAddr.getMsb() && packet.getRemoteAddress64().getLsb() == coordinatorAddr.getLsb();
}

void handleRx(ZBRxResponse& packet) {
	if(packet.getDataLength() == 0) {
		Serial.print("WARN\tgot emtpy packet from ");  Serial.print(packet.getRemoteAddress64().getMsb(), HEX); Serial.print(" "); Serial.println(packet.getRemoteAddress64().getLsb(), HEX);
		return;
	}
	switch(packet.getData()[0]) {
		case PKT_CHECKIN_RESPONSE:
			// todo signature check
			if(coordinatorAddr.getLsb() == ADDR_BCAST_LSB && coordinatorAddr.getMsb() == ADDR_BCAST_MSB) {
				coordinatorAddr.setMsb(packet.getRemoteAddress64().getMsb());
				coordinatorAddr.setLsb(packet.getRemoteAddress64().getLsb());
				Serial.print("INFO\tnew coordinator address "); Serial.print(coordinatorAddr.getMsb(), HEX); Serial.print(" "); Serial.println(coordinatorAddr.getLsb(), HEX);
			}
			if(packetFromCoordinator(packet)) {
				Serial.println("INFO\tgot checkin response");
				checkinLastRxMillis = curMillis;
			}
			else {
				Serial.print("WARN\tcheckin response from unknown source "); Serial.print(packet.getRemoteAddress64().getMsb(), HEX); Serial.print(" "); Serial.println(packet.getRemoteAddress64().getLsb(), HEX);
				return;
			}
			break;
		case PKT_ANIMATION_REQUEST:
			if(!packetFromCoordinator(packet)) {
				Serial.print("WARN\tanimation request from unknown source "); Serial.print(packet.getRemoteAddress64().getMsb(), HEX); Serial.print(" "); Serial.println(packet.getRemoteAddress64().getLsb(), HEX);
				return;
			}

			if(packet.getDataLength()<2) {
				Serial.println("WARN\tmalformed animation request");
				return;
			}

			dataIncomingLength = packet.getData()[1]<<8 | packet.getData()[2];
			if(dataIncomingLength > MAX_ANIMATION_LENGTH) {
				dataIncomingLength = MAX_ANIMATION_LENGTH;
			}
			for(int i=0; i<dataIncomingLength/3; i++) {
				data[dataNextSlot][i] = 0;
			}

			Serial.print("INFO\tanimation request, length "); Serial.println(dataIncomingLength);

			xbeeState = XBEESTATE_RECEIVE;
			dataTransferOffset = 0;
			dataTransferByte = 0;
			animationLastRxMillis = curMillis;

			xbee.send(animationResponsePacket);
			break;
		case PKT_ANIMATION_DATA:
			if(!packetFromCoordinator(packet)) {
				Serial.print("WARN\tanimation data from unknown source "); Serial.print(packet.getRemoteAddress64().getMsb(), HEX); Serial.print(" "); Serial.println(packet.getRemoteAddress64().getLsb(), HEX);
				return;
			}

			Serial.print("DEBUG\tanimation data, length "); Serial.println(packet.getDataLength()-1);
			for (int i=1; i<packet.getDataLength(); i++) {
				if(dataTransferOffset == dataIncomingLength) {
					break;
				}

				data[dataNextSlot][dataTransferOffset] = (data[dataNextSlot][dataTransferOffset] << 8) | packet.getData()[i];
				if(++dataTransferByte == 3) {
					dataTransferByte = 0;
					dataTransferOffset++;
				}
			}

			animationLastRxMillis = curMillis;

			if(dataTransferOffset*3 == dataIncomingLength) {
				Serial.println("INFO\tanimation successful transmission");
				xbee.send(animationSuccessPacket);
				xbeeState = XBEESTATE_WAIT;

				// (re)start animation
				curMillis = lastMillis = millis();
				curFrame = 0;
				frames = &data[dataNextSlot][0];
				dataNextSlot = (dataNextSlot+1) % 2;
				framesCount = dataIncomingLength/3/FRAMELENGTH;
				curFrameTime = frameTimeLeft = frames[curFrame*FRAMELENGTH + PIXELSNUM];

				Serial.print("curFrame "); Serial.println(curFrame);
				Serial.print("framesCount "); Serial.println(framesCount);
				Serial.print("curFrameTime "); Serial.println(curFrameTime);
				for(int i=0; i<dataIncomingLength/3; i++) {
					Serial.println(frames[i], HEX);
				}
			}

			break;
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
	Serial3.begin(19200);
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
	// this will trigger immediate checkin
	checkinLastRxMillis = checkinLastTxMillis = curMillis - COORDINATOR_HEARTBEAT_MILLIS - 1000;
}

void loop() {
	curMillis = millis();
	if(frames && framesCount) {
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
				break;
			default:
				Serial.print("WARN\tunhandled packet, api id "); Serial.println(packet.getApiId(), HEX);
		}
	}

	curMillis = millis();
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
	if (xbeeState == XBEESTATE_RECEIVE && (curMillis-animationLastRxMillis)>DATA_TRANSFER_TIMEOUT_MILLIS) {
		Serial.println("WARN\tdata receive timed outed");
		xbeeState = XBEESTATE_WAIT;
	}
}

