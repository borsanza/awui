#pragma once

namespace awui::Emulation::Chip8 {
	class Sound {
	  private:
		bool _playing;
		int _console_fd;

	  public:
		Sound();
		virtual ~Sound();

		void Play();
		void Stop();
	};
} // namespace awui::Emulation::Chip8
