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
					MasterSystem * _rom;
					Drawing::Image * _tiles;
					Drawing::Image * _colors;
					bool _show;
					float _width;

				public:
					DebuggerSMS();
					virtual ~DebuggerSMS();

					virtual bool IsClass(Classes objectClass) const override;

					virtual void OnTick(float deltaSeconds);

					virtual void OnPaint(OpenGL::GL * gl);

					void SetRom(MasterSystem * rom);
					bool GetShow() { return _show; }
					void SetShow(bool show) { _show = show; }
			};
		}
	}
}
