#pragma once

#include <awui/Windows/Forms/Button.h>

using namespace awui::Windows::Forms;

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace Windows {
		namespace Emulators {
			class MasterSystem;

			class DebuggerSMS : public Button {
				private:
					MasterSystem * m_rom;
					Drawing::Image * m_tiles;
					Drawing::Image * m_colors;
					bool m_show;
					float m_width;

				public:
					DebuggerSMS();
					virtual ~DebuggerSMS() = default;

					virtual bool IsClass(Classes objectClass) const override;

					virtual void OnTick(float deltaSeconds);

					virtual void OnPaint(OpenGL::GL * gl);

					void SetRom(MasterSystem * rom);
					bool GetShow() { return m_show; }
					void SetShow(bool show) { m_show = show; }
			};
		}
	}
}
