// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <IRremote.h>
#include "appleremote.h"

AppleRemote appleRemote;

void setup() {
//	appleRemote.setOutputSpeed(9600);
	appleRemote.setOutputSpeed(115200);
	appleRemote.setIRPin(6);

	appleRemote.setLogMode(true);
	appleRemote.setLedPin(13);
//	appleRemote.setBlink13Mode(true);
	appleRemote.init();
}

void loop() {
	appleRemote.loop();
}
