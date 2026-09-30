#pragma once

namespace awui::Emulation::Chip8 {
	class Sound {
	  private:
		bool m_playing;
		int m_consoleFd;

	  public:
		Sound();
		virtual ~Sound();

		void Play();
		void Stop();
	};
} // namespace awui::Emulation::Chip8
