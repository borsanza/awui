#pragma once

#include <cstdint>
#include <vector>

namespace awui::Emulation::Chip8 {
	// Zumbador del Chip-8: suena (onda cuadrada) mientras el temporizador de sonido es distinto de 0. Cada tick de 60 Hz
	// genera sus muestras y las encola en Common::AudioOutput (solo se oyen si es el emulador que está en juego)
	class Sound {
	  private:
		bool m_playing;
		double m_phase;			 // Posición dentro del periodo de la onda (0..1)
		double m_pendingSamples; // Fracción de muestra que queda para el siguiente tick
		std::vector<int16_t> m_samples;

	  public:
		static constexpr double Frequency = 440.0;
		static constexpr int16_t Level = 3000;

		Sound();
		~Sound();

		void Play();
		void Stop();
		inline bool IsPlaying() const { return m_playing; }

		// Genera y encola el sonido de un tick (seconds: lo que dura)
		void EndTick(double seconds);
	};
} // namespace awui::Emulation::Chip8
