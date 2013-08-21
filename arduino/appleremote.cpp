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

unsigned short RemoteKey::getId() const {
	return this->id;
}

unsigned char RemoteKey::getCommand() const {
	return this->command;
}

unsigned char RemoteKey::getRemoteId() const {
	return this->remoteId;
}

void RemoteKey::updateTime() {
	this->lastTime = millis();
}

unsigned long RemoteKey::getLastTime() const {
	return this->lastTime;
}

bool RemoteKey::isRepeatingKey() const {
	return ((this->id == 0xFFFF) && (this->remoteId == 0xFF) && (this->command == 0xFF));
}

void RemoteKey::setRepeatingKey() {
	this->id = 0xFFFF;
	this->remoteId = 0xFF;
	this->command = 0xFF;
}

bool RemoteKey::isAppleRemote() const {
	return ((this->id == 0x87EE) || (this->id == 0x87E0));
}

bool RemoteKey::isKey() const {
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

const char * RemoteKey::getName() const {
	if (this->id == 0x87E0) {
		switch (this->command) {
			// Menu + Play en mando blanco cambia el codigo del control remoto
			case 0x02: // Menu + Ok
			case 0x03:
				// Menu + Right
				return "REMOTE LINKED";
			case 0x04:
			case 0x05:
				// Menu + Left
				return "ACCEPT ALL REMOTES";
		}
	}

	if (this->id == 0x87EE) {
		switch (this->command) {
// Normales
			case 0x5F: // ¿?
			case 0x5E:
			case 0x04:
			case 0x05:
				return "PLAY/PAUSE";
			case 0x02:
			case 0x03:
				return "MENU";
			case 0x5C: // ¿?
			case 0x5D:
				return "OK";
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
				return "PLAY/PAUSE + RIGHT";
			case 0x15:
				// Play/Pause + Left
				return "PLAY/PAUSE + LEFT";
			case 0x0E:
				// Play/Pause + Up
				return "PLAY/PAUSE + UP";
			case 0x10:
				// Play/Pause + Down
				return "PLAY/PAUSE + DOWN";

// Especiales con Play/Pause (mando gris)
			case 0x6A:
				// Play/Pause + Right
				return "PLAY/PAUSE + RIGHT";
			case 0x69:
				// Play/Pause + Left
				return "PLAY/PAUSE + LEFT";
			case 0x65:
				// Play/Pause + Up
				return "PLAY/PAUSE + UP";
			case 0x66:
				// Play/Pause + Down
				return "PLAY/PAUSE + DOWN";
			case 0x63:
				// Play/Pause + Ok
				return "PLAY/PAUSE + OK";

// Especiales con Menu
			case 0x16:
			case 0x17:
				// Menu + Arriba
				return "CHANGE RESOLUTION";
			case 0x18:
			case 0x19:
				// Menu + Abajo
				return "RESET";
			case 0x60:
				// Menu + Play/Pause (mando gris)
				return "MENU + PLAY/PAUSE";
		}
	}

	return "UNKNOWN";
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

AppleRemote::AppleRemote() {
	this->ledPin = -1;
	this->outputSpeed = 9600;
	this->irpin = 6;
	this->blink13 = false;
	this->irrecv = 0;
	this->enableLog = false;
	this->linkedRemoteId = 0;
	this->lastTime = 0;
	this->lastState = false; // Unpressed
}

void AppleRemote::setLogMode(bool enabled) {
	this->enableLog = enabled;
}

void AppleRemote::setOutputSpeed(unsigned long speed) {
	this->outputSpeed = speed;
}

void AppleRemote::setIRPin(int irpin) {
	this->irpin = irpin;
}

void AppleRemote::setLedPin(int ledPin) {
	this->ledPin = ledPin;
}

void AppleRemote::setBlink13Mode(bool enabled) {
	this->blink13 = enabled;
}

void AppleRemote::init() {
	Serial.begin(this->outputSpeed);

	if (this->enableLog)
		Serial.println("STARTING APP");

	this->irrecv = new IRrecv(this->irpin);
	this->irrecv->enableIRIn();
	this->irrecv->blink13(this->blink13);
}

void AppleRemote::printCommand(const char * text, bool pressed) {
	if (pressed == this->lastState)
		return;

	if (this->ledPin != -1)
		digitalWrite(this->ledPin, pressed?HIGH:LOW);

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
	Serial.print(":");

	if (this->enableLog) {
		Serial.print(" ");
		Serial.print(text);
	}

	Serial.println();
}

void AppleRemote::sendCommand(const char * text, bool pressed) {
	this->lastTime = millis();
	this->printCommand(text, pressed);
}

bool AppleRemote::acceptRemote() const {
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
			this->printCommand("", 0);
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
		this->printCommand("", 0);
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
					this->sendCommand(actualKey.getName(), 1);
				}
				return;
			case 0x04:
			case 0x05:
				// Menu + Izq
				if (this->linkedRemoteId != 0) {
					this->linkedRemoteId = 0;
					this->sendCommand(actualKey.getName(), 1);
				}
				return;
			default:
				if (this->enableLog)
					this->sendCommand(actualKey.getName(), 1);
				break;
		}
	}

	if (this->actualKey.isAppleRemote() && this->acceptRemote())
		this->sendCommand(actualKey.getName(), 1);
}
