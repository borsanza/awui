#pragma once

namespace awui::Emulation::Common {
	// Ajustes de sonido comunes a todos los emuladores (los cambia el menú de ajustes)
	class AudioSettings {
	  private:
		static inline bool m_enabled = true;
		static inline int m_volume = 100; // Porcentaje: 0-100

	  public:
		static inline void SetEnabled(bool enabled) { m_enabled = enabled; }
		static inline void SetVolume(int volume) { m_volume = (volume < 0) ? 0 : ((volume > 100) ? 100 : volume); }

		// Ganancia final en porcentaje (0 si el sonido está desactivado)
		static inline int GetGain() { return m_enabled ? m_volume : 0; }
	};
} // namespace awui::Emulation::Common
