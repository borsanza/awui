#pragma once

#include <SDL.h>
#include <stdint.h>

// 48000, 44100, 22050, 11025
#define SOUNDFREQ 44100

namespace awui::Emulation::MasterSystem {
	class Sound;

	// Salida de audio mediante la cola de SDL (SDL_QueueAudio): el emulador genera las
	// muestras de cada frame y las encola, sin callback ni hilos compartiendo buffers.
	class SoundSDL {
	  private:
		SDL_AudioDeviceID m_audioDevice;
		Sound *m_playing;
		static uint8_t m_disabledChannels;

		SoundSDL(const SoundSDL &) = delete;
		SoundSDL &operator=(const SoundSDL &) = delete;

		int GetQueuedSamples() const;

	  public:
		SoundSDL();
		~SoundSDL();

		static SoundSDL &Instance();

		void SetPlayingSound(Sound *sound);
		inline bool IsPlaying(const Sound *sound) const { return m_audioDevice != 0 && sound == m_playing; }

		// samples: estéreo intercalado (izquierda, derecha); count: número de parejas
		void Queue(Sound *sound, const int16_t *samples, int count);
		double GetRateAdjust() const;

		static void ToggleChannel(int channel);
		static void SetChannelEnabled(int channel, bool enabled);
		static inline bool IsChannelEnabled(int channel) { return (m_disabledChannels & (1 << channel)) == 0; }
	};
} // namespace awui::Emulation::MasterSystem
