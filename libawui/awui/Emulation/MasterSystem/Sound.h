#pragma once

#include <stdint.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100
#define SOUNDSAMPLES 512
// #define SOUNDSAMPLES 1024
// #define SOUNDSAMPLES 2048

#define TOTALFRAMES 3
#define SOUNDBUFFER (SOUNDSAMPLES * TOTALFRAMES)

#include <awui/Object.h>

namespace awui::Emulation::MasterSystem {
	struct Sample {
		int8_t _volume;
		uint16_t _tone;
		uint8_t _changeTone : 1;
		uint8_t _changeVolume : 1;
	};

	struct Channel {
		bool _useModulation;
		uint16_t _register;

		uint16_t _tone;	 // 10 bits tono y 3 bits noise
		uint8_t _volume; // 4 bit
		unsigned int _fase;

		Sample _buffer[SOUNDBUFFER];
		Sample _last;
		unsigned int _count;
	};

	class Motherboard;

	class Sound : public Object {
	  private:
		uint16_t m_channel;
		uint8_t m_type; // 1: Volumen, 0: Tone/Noise
		Motherboard *m_cpu;

		int GetPosBuffer(Motherboard *cpu);

	  public:
		uint16_t m_noiseData;
		Channel m_channels[4];

		Sound();

		inline void SetCPU(Motherboard *cpu) { m_cpu = cpu; }
		inline Motherboard *GetCPU() const { return m_cpu; }

		void WriteByte(Motherboard *cpu, uint8_t value);
	};
} // namespace awui::Emulation::MasterSystem
