#pragma once

#include <SDL.h>
#include <stdint.h>

namespace awui::Emulation::Spectrum {
	class Sound;

	// Salida de audio mediante la cola de SDL (SDL_QueueAudio), como en Master System: cada frame emulado
	// genera sus muestras y se encolan. Solo suena el Sound que esté en juego (SetPlayingSound)
	class SoundSDL {
	  private:
		SDL_AudioDeviceID m_audioDevice;
		Sound *m_playing;

		SoundSDL();
		SoundSDL(const SoundSDL &) = delete;
		SoundSDL &operator=(const SoundSDL &) = delete;

		int GetQueuedSamples() const;

	  public:
		~SoundSDL();

		static SoundSDL *Instance();

		void SetPlayingSound(Sound *sound);
		inline bool IsPlaying(const Sound *sound) const { return (m_audioDevice != 0) && (sound == m_playing); }

		// samples: mono; count: número de muestras
		void Queue(Sound *sound, const int16_t *samples, int count);
		double GetRateAdjust() const;
	};
} // namespace awui::Emulation::Spectrum
