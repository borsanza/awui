// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef AppleRemote_h
#define AppleRemote_h

class RemoteKey {
	private:
		unsigned short id;
		unsigned char command;
		unsigned char remoteId;
		unsigned long lastTime;

	public:
		RemoteKey();
		void decode(unsigned long command);
		
		unsigned short getId() const;
		unsigned char getCommand() const;
		unsigned char getRemoteId() const;
		bool isRepeatingKey() const;
		bool isKey() const;
		bool isAppleRemote() const;
		unsigned long getLastTime() const;

		void updateTime();
		void setRepeatingKey();
		void setNoKey();
		
		const char * getName() const;
};

class IRrecv;

class AppleRemote {
	private:
		RemoteKey actualKey;

		IRrecv *irrecv;

		bool blink13;
		bool enableLog;
		bool lastState;
		unsigned long lastTime;
		unsigned long outputSpeed;
		unsigned char linkedRemoteId;
		int irpin;
		int ledPin;

		void sendCommand(const char * value, bool pressed);
		void printCommand(const char * value, bool pressed);
		bool acceptRemote() const;

	public:
		AppleRemote();
		void setOutputSpeed(unsigned long speed);
		void setIRPin(int irpin);
		void setLedPin(int ledPin);
		void setBlink13Mode(bool enabled);
		void setLogMode(bool enabled);
		void init();
		void loop();
};

#endif
