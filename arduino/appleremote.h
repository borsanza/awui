// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef AppleRemote_h
#define AppleRemote_h

class IRrecv;

class RemoteKey {
	private:
		unsigned short id;
		unsigned char command;
		unsigned char remoteId;
		unsigned long lastTime;

	public:
		RemoteKey();
		void decode(unsigned long command);
		
		unsigned short getId();
		unsigned char getCommand();
		unsigned char getRemoteId();

		void updateTime();
		unsigned long getLastTime();
		int isRepeatingKey();
		int isKey();
		void setRepeatingKey();
		void setNoKey();
		int isAppleRemote();
		
		const char * getName();
};

class AppleRemote {
	private:
		unsigned long outputSpeed;
		int pin;
		int blink13;
		int enableLog;
		int lastState;
		
		RemoteKey actualKey;

		unsigned char linkedRemoteId;
		int canRepeat;
		unsigned long lastTime;

		IRrecv *irrecv;
		
		void sendCommand(const char * value, int pressed);
		void printCommand(const char * value, int pressed);

		int isAppleRemote();
		int acceptRemote();

	public:
		AppleRemote();
		void setOutputSpeed(unsigned long speed);
		void setPin(int pin);
		void setBlink13Mode(int enabled);
		void setLogMode(int enabled);
		void init();
		void loop();
};

#endif
