#ifndef _AWUI_WINDOWS_EMULATORS_DEBUGGERSMS_H
#define _AWUI_WINDOWS_EMULATORS_DEBUGGERSMS_H

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

				public:
					DebuggerSMS();
					virtual ~DebuggerSMS();

					virtual int IsClass(Classes::Enum objectClass) const;

					virtual void OnTick();

					virtual void OnPaint(OpenGL::GL * gl);
					virtual bool OnKeyPress(Keys::Enum key);
					
					void SetRom(MasterSystem * rom);
			};
		}
	}
}

#endif