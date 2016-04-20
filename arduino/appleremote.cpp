// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#if ARDUINO < 100
	#include <WProgram.h>
#else
	#include <Arduino.h>
#endif

#include <EEPROM.h>
#include <IRremote.h>

#include "appleremote.h"

RemoteKey::RemoteKey() {
	this->id = 0;
	this->remoteId = 0;
	this->lastTime = 0;

	this->setNoKey();
	this->resetCount();
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
//	unsigned int diff = millis() - this->lastTime;
//	Serial.println(diff);
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

unsigned int RemoteKey::getCount() const {
	return this->count;
}

void RemoteKey::resetCount() {
	this->count = 0;
}

void RemoteKey::incCount() {
	this->count++;
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
			case 0x02:
			case 0x03:
				return "MENU";
			case 0x04:
			case 0x05:
			case 0x5E:
			case 0x5F: // ¿?
				return "PLAY/PAUSE";
			case 0x5C: // ¿?
			case 0x5D:
				return "OK";
			case 0x06:
			case 0x07:
				return "RIGHT";
			case 0x08:
			case 0x09:
				return "LEFT";
			case 0x0A:
			case 0x0B:
				return "UP";
			case 0x0C:
			case 0x0D:
				return "DOWN";

// Especiales con Play/Pause (mando blanco)
			case 0x0E:
			case 0x0F:
				// Play/Pause + Up
				return "PLAY/PAUSE + UP";
			case 0x10:
			case 0x11:
				// Play/Pause + Down
				return "PLAY/PAUSE + DOWN";
			case 0x12:
			case 0x13:
				// Play/Pause + Right
				return "PLAY/PAUSE + RIGHT";
			case 0x14:
			case 0x15:
				// Play/Pause + Left
				return "PLAY/PAUSE + LEFT";

// Especiales con Menu
			case 0x16:
			case 0x17:
				// Menu + Arriba
				return "CHANGE RESOLUTION";
			case 0x18:
			case 0x19:
				// Menu + Abajo
				return "RESET";

// Especiales con Play/Pause (mando gris)
			case 0x60:
			case 0x61:
				// Play/Pause + Menu
				return "MENU + PLAY/PAUSE";
			case 0x62:
			case 0x63:
				// Play/Pause + Ok
				return "PLAY/PAUSE + OK";
			case 0x64:
			case 0x65:
				// Play/Pause + Up
				return "PLAY/PAUSE + UP";
			case 0x66:
			case 0x67:
				// Play/Pause + Down
				return "PLAY/PAUSE + DOWN";
			case 0x68:
			case 0x69:
				// Play/Pause + Left
				return "PLAY/PAUSE + LEFT";
			case 0x6A:
			case 0x6B:
				// Play/Pause + Right
				return "PLAY/PAUSE + RIGHT";
		}
	}

	return "UNKNOWN";
}

const char * RemoteKey::getCodeLibrary() const {
	if (this->id == 0x87E0) {
		switch (this->command) {
			// Menu + Play en mando blanco cambia el codigo del control remoto
			case 0x02: // Menu + Ok or Menu + Right
			case 0x03:
				return "51";
			case 0x04:
			case 0x05:
				// Menu + Left
				return "52";
		}
	}

	if (this->id == 0x87EE) {
		switch (this->command) {
// Normales
			case 0x02:
			case 0x03:
				return "10";
			case 0x04:
			case 0x05:
				return "11";
			case 0x5E:
			case 0x5F: // ¿?
				return "17";
			case 0x5C: // ¿?
			case 0x5D:
				return "16";
			case 0x06:
			case 0x07:
				return "12";
			case 0x08:
			case 0x09:
				return "13";
			case 0x0A:
			case 0x0B:
				return "14";
			case 0x0C:
			case 0x0D:
				return "15";

// Especiales con Play/Pause (mando blanco)
			case 0x0E:
			case 0x0F:
				// Play/Pause + Up
				return "21";
			case 0x10:
			case 0x11:
				// Play/Pause + Down
				return "22";
			case 0x12:
			case 0x13:
				// Play/Pause + Right
				return "23";
			case 0x14:
			case 0x15:
				// Play/Pause + Left
				return "24";

// Especiales con Menu
			case 0x16:
			case 0x17:
				// Menu + Arriba
				return "31";
			case 0x18:
			case 0x19:
				// Menu + Abajo
				return "32";

// Especiales con Play/Pause (mando gris)
			case 0x60:
			case 0x61:
				// Play/Pause + Menu
				return "41";
			case 0x62:
			case 0x63:
				// Play/Pause + Ok
				return "42";
			case 0x64:
			case 0x65:
				// Play/Pause + Up
				return "43";
			case 0x66:
			case 0x67:
				// Play/Pause + Down
				return "44";
			case 0x68:
			case 0x69:
				// Play/Pause + Left
				return "45";
			case 0x6A:
			case 0x6B:
				// Play/Pause + Right
				return "46";
		}
	}

	return "FF";
}

void RemoteKey::verbose(bool endline) {
	Serial.print(this->id, HEX);
	Serial.print(":");
	if (this->command < 16)
		Serial.print("0");
	Serial.print(this->command, HEX);
	Serial.print(":");
	if (this->remoteId < 16)
		Serial.print("0");
	Serial.print(this->remoteId, HEX);

	if (endline)
		Serial.println();
}

bool RemoteKey::isDoubleButton() const {
	if (this->id == 0x87E0)
		return true;

	if (this->id == 0x87EE) {
		if ((this->command >= 0x5C) && (this->command <= 0x5F))
			return false;
	
		if (this->command >= 0x0E)
			return true;
	}

	return false;
}

bool RemoteKey::isSpecialKey() const {
	if (this->isDoubleButton())
		return true;

	if ((this->id == 0x87EE) && (this->command >= 0x5C) && (this->command <= 0x5F))
		return true;

	return false;
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

	this->loadLinkedRemoteId();
}

void AppleRemote::printCommand(const char * text, bool pressed) {
	if (pressed == this->lastState)
		return;

	if (this->ledPin != -1)
		digitalWrite(this->ledPin, pressed?HIGH:LOW);

	this->lastState = pressed;

	Serial.print(this->actualKey.getCodeLibrary());
	Serial.print(":");
	Serial.print(pressed);
	Serial.print(":");

	this->actualKey.verbose(false);

	Serial.print(":");
	Serial.print(this->actualKey.getCount());
	Serial.print(":");

	if (this->enableLog) {
		Serial.print(" ");
		Serial.print(text);
	}

	Serial.println();
}

void AppleRemote::sendCommand() {
	this->lastTime = millis();
	this->printCommand(actualKey.getName(), true);
}

bool AppleRemote::acceptRemote() const {
	if ((this->actualKey.getId() == 0xFFFF) && (this->actualKey.getRemoteId() == 0xFF) && (this->actualKey.getCommand() == 0xFF))
		return true;

	if (this->linkedRemoteId == this->actualKey.getRemoteId())
		return true;

	if (this->linkedRemoteId == -1)
		return true;
	
	return false;
}

void AppleRemote::setLinkedRemoteId(int linkedRemoteId) {
	this->linkedRemoteId = linkedRemoteId;

	EEPROM.write(0, (linkedRemoteId != -1) ? 1 : 0);
	EEPROM.write(1, linkedRemoteId);
}

void AppleRemote::loadLinkedRemoteId() {
	if (EEPROM.read(0) == 1) {
		this->linkedRemoteId = EEPROM.read(1);
		if (this->enableLog) {
			Serial.print("ACCEPT REMOTE: ");
			Serial.println(this->linkedRemoteId, HEX);
		}
	} else {
		this->linkedRemoteId = -1;
		if (this->enableLog)
			Serial.println("ACCEPT ALL REMOTES");
	}
}

void AppleRemote::loop() {
	// Comprobar si ha pasado un tiempo prudencial para descartar presiones
	if (this->actualKey.isKey()) {
		unsigned long diff = millis() - this->actualKey.getLastTime();

		// 109 * 1.5 = 164
		int temp = 164;
		if ((this->actualKey.getCount() == 0) && !this->actualKey.isSpecialKey())
			// 42 * 1.5 = 63
			temp = 63;
		
		if (diff > temp) {
			this->printCommand("", 0);
			this->actualKey.setNoKey();
			this->actualKey.resetCount();
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
//	readKey.verbose(true);
//	return;

	// Convertir botones ignorables en repetibles
	{
		// Si es un comando de doble pulsacion es repetitivo
		if (this->actualKey.isDoubleButton())
			if (readKey.getCommand() == this->actualKey.getCommand())
				readKey.setRepeatingKey();

		// Si son comandos de compatibilidad se convierten a repetitivos
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
		if (this->actualKey.isKey()) {
			this->actualKey.updateTime();
			this->actualKey.incCount();
		}
		return;
	}

	if (!readKey.isAppleRemote())
		return;
	
	if (this->actualKey.isKey()) {
		this->printCommand("", 0);
		this->actualKey.setNoKey();
	}

	this->actualKey = readKey;
	this->actualKey.updateTime();
	this->actualKey.resetCount();

	if (this->actualKey.getId() == 0x87E0) {
		switch (this->actualKey.getCommand()) {
			// Menu + Play en mando blanco cambia el codigo del control remoto
			case 0x02: // Menu + Ok
			case 0x03:
				// Menu + Der
				if (this->linkedRemoteId != this->actualKey.getRemoteId()) {
					this->setLinkedRemoteId(this->actualKey.getRemoteId());
					this->sendCommand();
				}
				return;
			case 0x04:
			case 0x05:
				// Menu + Izq
				if (this->linkedRemoteId != -1) {
					this->setLinkedRemoteId(-1);
					this->sendCommand();
				}
				return;
			default:
				if (this->enableLog)
					this->sendCommand();
				break;
		}
	}

	if (this->actualKey.isAppleRemote() && this->acceptRemote())
		this->sendCommand();
}


