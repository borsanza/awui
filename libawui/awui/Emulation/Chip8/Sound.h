#pragma once

namespace awui {
	namespace Emulation {
		namespace Chip8 {
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
		}
	}
}
