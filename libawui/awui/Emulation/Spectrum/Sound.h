#pragma once

#include <stdint.h>
#include <vector>

#define SOUNDFREQ 44100

#include <awui/Object.h>

namespace awui::Emulation::Spectrum {
	class Motherboard;

	// Altavoz del Spectrum (bits EAR y MIC del puerto 0xFE). Durante el frame se apuntan los cambios de nivel con
	// su posición dentro del frame; al acabarlo se convierten en muestras y se encolan en SoundSDL
	class Sound : public Object {
	  private:
		struct Change {
			double position; // 0..1 dentro del frame
			int16_t level;
		};

		Motherboard *m_cpu;
		std::vector<Change> m_changes;
		std::vector<int16_t> m_samples;
		int16_t m_level;		 // Nivel al empezar el frame
		double m_pendingSamples; // Fracción de muestra que queda para el siguiente frame
		float m_dcIn;			 // Filtro que quita la continua (el altavoz en reposo no está a cero)
		float m_dcOut;

	  public:
		Sound();

		inline void SetCPU(Motherboard *cpu) { m_cpu = cpu; }
		inline Motherboard *GetCPU() { return m_cpu; }

		void WriteSound(Motherboard *cpu, int value);

		// Genera las muestras del frame que acaba de emularse (duración en segundos reales)
		void EndFrame(double seconds, bool silent);
	};
} // namespace awui::Emulation::Spectrum
