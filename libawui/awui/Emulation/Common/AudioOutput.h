#pragma once

#include <SDL.h>
#include <stdint.h>

namespace awui::Emulation::Common {
	// Salida de audio común a todos los emuladores: un único dispositivo de SDL, en estéreo, alimentado con la cola
	// de SDL (SDL_QueueAudio). Cada emulador genera las muestras de su frame y las encola, sin callback ni hilos
	// compartiendo buffers. Solo suena la fuente que esté en juego (SetPlaying); el volumen se aplica aquí
	class AudioOutput {
	  private:
		SDL_AudioDeviceID m_audioDevice;
		const void *m_playing;

		AudioOutput();
		AudioOutput(const AudioOutput &) = delete;
		AudioOutput &operator=(const AudioOutput &) = delete;

		int GetQueuedFrames() const;

	  public:
		// 48000, 44100, 22050, 11025
		static constexpr int Frequency = 44100;

		~AudioOutput();

		static AudioOutput &Instance();

		// La fuente que suena (el Sound del emulador en juego) o nullptr
		void SetPlaying(const void *source);
		inline bool IsPlaying(const void *source) const { return (m_audioDevice != 0) && (source == m_playing); }

		// samples: frames muestras por canal; con 2 canales, intercaladas (izquierda, derecha). En mono se envía
		// la misma muestra a los dos lados
		void Queue(const void *source, const int16_t *samples, int frames, int channels);

		// Para que la cola no crezca ni se vacíe: > 1 cuando sobran muestras encoladas (hay que generar menos),
		// < 1 cuando faltan. Se multiplica por los ciclos por muestra o se divide entre las muestras por segundo
		double GetRateAdjust() const;

		// Si ya hay en cola todo lo que se quiere tener: quien genera más deprisa de lo normal (el avance rápido)
		// puede saltarse muestras
		bool IsQueueFull() const;
	};
} // namespace awui::Emulation::Common
