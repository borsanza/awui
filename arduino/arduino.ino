// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <IRremote.h>
#include "appleremote.h"

AppleRemote appleRemote;

void setup() {
	appleRemote.setLogMode(true);
//	appleRemote.setOutputSpeed(9600);
	appleRemote.setOutputSpeed(115200);
	appleRemote.setPin(6);
	appleRemote.setBlink13Mode(true);
	appleRemote.init();
}

void loop() {
	appleRemote.loop();
}
