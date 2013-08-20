// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#if ARDUINO < 100
	#include <WProgram.h>
#else
	#include <Arduino.h>
#endif

#include <IRremote.h>

#include "appleremote.h"

RemoteKey::RemoteKey() {
	this->id = 0;
	this->remoteId = 0;
	this->lastTime = 0;

	this->setNoKey();
}

unsigned short RemoteKey::getId() {
	return this->id;
}

unsigned char RemoteKey::getCommand() {
	return this->command;
}

unsigned char RemoteKey::getRemoteId() {
	return this->remoteId;
}

void RemoteKey::updateTime() {
	this->lastTime = millis();
}

unsigned long RemoteKey::getLastTime() {
	return this->lastTime;
}

int RemoteKey::isRepeatingKey() {
	return ((this->id == 0xFFFF) && (this->remoteId == 0xFF) && (this->command == 0xFF));
}

void RemoteKey::setRepeatingKey() {
	this->id = 0xFFFF;
	this->remoteId = 0xFF;
	this->command = 0xFF;
}

int RemoteKey::isAppleRemote() {
	return ((this->id == 0x87EE) || (this->id == 0x87E0));
}

int RemoteKey::isKey() {
	return this->command != 0xFE;
}

void RemoteKey::setNoKey() {
	this->command = 0xFE;
}

void RemoteKey::decode(unsigned long command) {
	unsigned long command3 = 0;
	for (int i=0; i<32; i++) {
		command3 = command3 << 1;
		command3 |= command & 1;
		command = command >> 1;
	}

	this->id = command3 & 0xFFFF;
	this->command = (command3 >> 16) & 0xFF;
	this->remoteId = (command3 >> 24) & 0xFF;
}

const char * RemoteKey::getName() {
	if (this->id == 0x87E0) {
		switch (this->command) {
			// Menu + Play en mando blanco cambia el codigo del control remoto
			case 0x02: // Menu + Ok
			case 0x03:
				// Menu + Right
				return "Remote linked";
			case 0x04:
			case 0x05:
				// Menu + Left
				return "Accept all remotes";
		}
	}

	if (this->id == 0x87EE) {
		switch (this->command) {
// Normales
			case 0x5F: // ¿?
			case 0x5E:
			case 0x04:
			case 0x05:
				return "Play/Pause";
			case 0x02:
			case 0x03:
				return "Menu";
			case 0x5C: // ¿?
			case 0x5D:
				return "Ok";
			case 0x07:
			case 0x06:
				return "RIGHT";
			case 0x08:
			case 0x09:
				return "LEFT";
			case 0x0B:
			case 0x0A:
				return "UP";
			case 0x0D:
			case 0x0C:
				return "DOWN";

// Especiales con Play/Pause (mando blanco)
			case 0x13:
				// Play/Pause + Right
				return "Play/Pause + Right";
			case 0x15:
				// Play/Pause + Left
				return "Play/Pause + Left";
			case 0x0E:
				// Play/Pause + Up
				return "Play/Pause + Up";
			case 0x10:
				// Play/Pause + Down
				return "Play/Pause + Down";

// Especiales con Play/Pause (mando gris)
			case 0x6A:
				// Play/Pause + Right
				return "Play/Pause + Right";
			case 0x69:
				// Play/Pause + Left
				return "Play/Pause + Left";
			case 0x65:
				// Play/Pause + Up
				return "Play/Pause + Up";
			case 0x66:
				// Play/Pause + Down
				return "Play/Pause + Down";
			case 0x63:
				// Play/Pause + Ok
				return "Play/Pause + Ok";

// Especiales con Menu
			case 0x16:
			case 0x17:
				// Menu + Arriba
				return "Change resolution";
			case 0x18:
			case 0x19:
				// Menu + Abajo
				return "Reset";
			case 0x60:
				// Menu + Play/Pause (mando gris)
				return "Menu + Play/Pause";
		}
	}

	return "Unknown";
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

AppleRemote::AppleRemote() {
	this->outputSpeed = 9600;
	this->pin = 6;
	this->blink13 = false;
	this->irrecv = 0;
	this->enableLog = false;
	this->linkedRemoteId = 0;
	this->canRepeat = false;
	this->lastTime = 0;
	this->lastState = 1; // Unpressed
}

void AppleRemote::setLogMode(int enabled) {
	this->enableLog = enabled;
}

void AppleRemote::setOutputSpeed(unsigned long speed) {
	this->outputSpeed = speed;
}

void AppleRemote::setPin(int pin) {
	this->pin = pin;
}

void AppleRemote::setBlink13Mode(int enabled) {
	this->blink13 = enabled;
}

void AppleRemote::init() {
	Serial.begin(this->outputSpeed);

	if (this->enableLog)
		Serial.println("Starting app");

	this->irrecv = new IRrecv(this->pin);
	this->irrecv->enableIRIn();
	this->irrecv->blink13(this->blink13);
}

void AppleRemote::printCommand(const char * text, int pressed) {
	if (pressed == this->lastState)
		return;

	this->lastState = pressed;

	Serial.print(this->actualKey.getId(), HEX);
	Serial.print(":");
	if (this->actualKey.getCommand() < 16)
		Serial.print("0");
	Serial.print(this->actualKey.getCommand(), HEX);
	Serial.print(":");
	Serial.print(this->actualKey.getRemoteId(), HEX);
	Serial.print(":");
	Serial.print(pressed);
	Serial.print(": ");
	Serial.print(text);
	Serial.println();
}

void AppleRemote::sendCommand(const char * text, int pressed) {
	this->lastTime = millis();
	this->printCommand(text, pressed);
}

int AppleRemote::acceptRemote() {
	if ((this->actualKey.getId() == 0xFFFF) && (this->actualKey.getRemoteId() == 0xFF) && (this->actualKey.getCommand() == 0xFF))
		return true;

	if (this->linkedRemoteId == this->actualKey.getRemoteId())
		return true;

	if (this->linkedRemoteId == 0)
		return true;
	
	return false;
}

void AppleRemote::loop() {
	// Comprobar si ha pasado un tiempo prudencial para descartar presiones
	if (this->actualKey.isKey()) {
		unsigned long diff = millis() - this->actualKey.getLastTime();
		// 109 * 1.5
		if (diff > 164) {
			this->printCommand("KeyUp", 1);
			this->actualKey.setNoKey();
		}
	}

	// Obtener nuevos codigos
	decode_results results;
	if (!irrecv->decode(&results))
		return;
	irrecv->resume();

	// Decodificar el boton
	RemoteKey readKey;
	readKey.decode(results.value);

	// Convertir botones ignorables en repetibles
	{
		if (this->actualKey.getId() == 0x87E0) {
			switch (this->actualKey.getCommand()) {
				case 0x02:
				case 0x03:
				case 0x04:
				case 0x05:
					if (readKey.getCommand() == this->actualKey.getCommand())
						readKey.setRepeatingKey();
					break;
			}
		}

		switch (this->actualKey.getCommand()) {
			case 0x5C:
			case 0x5F:
				if (readKey.getCommand() == 0x05)
					readKey.setRepeatingKey();
				break;
			case 0x5D:
			case 0x5E:
				if (readKey.getCommand() == 0x04)
					readKey.setRepeatingKey();
				break;
		}
	}
	
	// Mirar si es un codigo de 
	if (readKey.isRepeatingKey()) {
		if (this->actualKey.isKey())
			this->actualKey.updateTime();
		return;
	}

	if (!readKey.isAppleRemote())
		return;
	
	if (this->actualKey.isKey()) {
		this->printCommand("KeyUp", 1);
		this->actualKey.setNoKey();
	}

	this->actualKey = readKey;
	actualKey.updateTime();

	if (this->actualKey.getId() == 0x87E0) {
		switch (this->actualKey.getCommand()) {
			// Menu + Play en mando blanco cambia el codigo del control remoto
			case 0x02: // Menu + Ok
			case 0x03:
				// Menu + Der
				if (this->linkedRemoteId != this->actualKey.getRemoteId()) {
					this->linkedRemoteId = this->actualKey.getRemoteId();
					this->sendCommand("Remote linked", 0);
				}
				return;
			case 0x04:
			case 0x05:
				// Menu + Izq
				if (this->linkedRemoteId != 0) {
					this->linkedRemoteId = 0;
					this->sendCommand("Accept all remotes", 0);
				}
				return;
			default:
				if (this->enableLog)
					this->sendCommand("Unknown", 0);
				break;
		}

		this->canRepeat = false;
	}

	if (this->actualKey.isAppleRemote() && this->acceptRemote()) {
		unsigned long diferencia;
		boolean repeat = true;
		this->sendCommand(actualKey.getName(), 0);

		this->canRepeat = repeat;
	}
}
