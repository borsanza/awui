// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <EEPROM.h>
#include <IRremote.h>
#include "appleremote.h"

AppleRemote appleRemote;

void setup() {
//	appleRemote.setOutputSpeed(9600);
//	appleRemote.setOutputSpeed(19200);
//	appleRemote.setOutputSpeed(57600);
	appleRemote.setOutputSpeed(115200);
	appleRemote.setIRPin(6);

	appleRemote.setLogMode(true);
	appleRemote.setLedPin(13);
//	appleRemote.setBlink13Mode(true);
	appleRemote.init();
}

void loop() {
	static int oldSec = 5;
	unsigned long time = millis();
	int sec = (time / 1000) % 60;

	if (sec != oldSec) {
		oldSec = sec;
		Serial.print("TICK: ");
		Serial.println(sec);
	}

	appleRemote.loop();
}
