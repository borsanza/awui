#include <IRremote.h>

#define PIN 6
#define LOGGING 0

IRrecv irrecv(PIN);
unsigned long _lastTime = 0;
boolean _canRepeat = false;

word _id = 0;
byte _command = 0;
byte _remoteid = 0;
word _linkedRemoteId = 0;

void setup() {
	Serial.begin(9600);
	
	if (LOGGING)
		Serial.println("Starting app");

	irrecv.enableIRIn();
//	irrecv.blink13(true);
}

void sendCommand(const char * text) {
	_lastTime = millis();

	Serial.print(_id, HEX);
	Serial.print(":");
	Serial.print(_command, HEX);
	Serial.print(":");
	Serial.print(_remoteid, HEX);
	Serial.print(": ");
	Serial.print(text);
	Serial.println();
}

void extractCommand(unsigned long command2) {
	unsigned long command = 0;
	for (int i=0; i<32; i++) {
		command = command << 1;
		command |= command2 & 1;
		command2 = command2 >> 1;
	}

	_id = command;
	_command = command >> 16;
	_remoteid = command >> 24;
}

boolean isAppleRemote() {
	return ((_id == 0x87EE) || (_id == 0xFFFF));
}

boolean acceptRemote() {
	if ((_id == 0xFFFF) && (_remoteid == 0xFF) && (_command == 0xFF))
		return true;

	if (_linkedRemoteId == _remoteid)
		return true;

	if (_linkedRemoteId == 0)
		return true;
	
	return false;
}

decode_results results;

void loop() {
	if (!irrecv.decode(&results))
		return;

	// Receive the next value
	irrecv.resume();

	extractCommand(results.value);

	if (_id == 0x87E0) {
		switch (_command) {
			// Menu + Play en mando blanco cambia el codigo del control remoto
			case 0x02: // Menu + Ok
			case 0x03:
				// Menu + Der
				if (_linkedRemoteId != _remoteid) {
					_linkedRemoteId = _remoteid;
					sendCommand("Remote linked");
				}
				return;
			case 0x04:
			case 0x05:
				// Menu + Izq
				if (_linkedRemoteId != 0) {
					_linkedRemoteId = 0;
					sendCommand("Accept all remotes");
				}
				return;
			default:
				if (LOGGING)
					sendCommand("Unknown");
				break;
		}
		_canRepeat = false;
	}

	if (isAppleRemote() && acceptRemote()) {
		unsigned long diferencia;
		boolean repeat = true;

		switch (_command) {
// Normales
			case 0x5F: // ¿?
			case 0x5E:
			case 0x04:
			case 0x05:
				sendCommand("Play/Pause");
				break;
			case 0x02:
			case 0x03:
				sendCommand("Menu");
				break;
			case 0x5C: // ¿?
			case 0x5D:
				sendCommand("Ok");
				break;
			case 0x07:
			case 0x06:
				sendCommand("RIGHT");
				break;
			case 0x08:
			case 0x09:
				sendCommand("LEFT");
				break;
			case 0x0B:
			case 0x0A:
				sendCommand("UP");
				break;
			case 0x0D:
			case 0x0C:
				sendCommand("DOWN");
				break;

// Especiales con Play/Pause (mando blanco)
			case 0x13:
				// Play/Pause + Right
				sendCommand("Play/Pause + Right");
				break;
			case 0x15:
				// Play/Pause + Left
				sendCommand("Play/Pause + Left");
				break;
			case 0x0E:
				// Play/Pause + Up
				sendCommand("Play/Pause + Up");
				break;
			case 0x10:
				// Play/Pause + Down
				sendCommand("Play/Pause + Down");
				break;

// Especiales con Play/Pause (mando gris)
			case 0x6A:
				// Play/Pause + Right
				sendCommand("Play/Pause + Right");
				break;
			case 0x69:
				// Play/Pause + Left
				sendCommand("Play/Pause + Left");
				break;
			case 0x65:
				// Play/Pause + Up
				sendCommand("Play/Pause + Up");
				break;
			case 0x66:
				// Play/Pause + Down
				sendCommand("Play/Pause + Down");
				break;
			case 0x63:
				// Play/Pause + Ok
				sendCommand("Play/Pause + Ok");
				break;

// Especiales con Menu
			case 0x16:
			case 0x17:
				// Menu + Arriba
				sendCommand("Change resolution");
				break;
			case 0x18:
			case 0x19:
				// Menu + Abajo
				sendCommand("Reset");
				break;
			case 0x60:
				// Menu + Play/Pause (mando gris)
				sendCommand("Menu + Play/Pause");
				break;
			case 0xFF:
				diferencia = millis() - _lastTime;

				if ((diferencia < 150) && _canRepeat)
					sendCommand("Repeat");
				break;
			default:
				repeat = false;
				if (LOGGING)
					sendCommand("Unknown");
				break;
		}

		_canRepeat = repeat;
	}
}
