#pragma once

#include <awui/Windows/Forms/Button.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace Windows::Emulators {
		class MasterSystem;

		class DebuggerSMS : public Forms::Button {
		  private:
			MasterSystem *m_rom;
			Drawing::Image *m_tiles;
			Drawing::Image *m_colors;
			bool m_show;
			float m_width;

		  public:
			DebuggerSMS();
			virtual ~DebuggerSMS() = default;

			virtual void OnTick(float deltaSeconds);

			virtual void OnPaint(OpenGL::GL *gl);

			void SetRom(MasterSystem *rom);
			bool GetShow() const { return m_show; }
			void SetShow(bool show) { m_show = show; }
		};
	} // namespace Windows::Emulators
} // namespace awui
