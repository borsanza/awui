#ifndef _AWUI_WINDOWS_EMULATORS_ARCADECONTAINER_H
#define _AWUI_WINDOWS_EMULATORS_ARCADECONTAINER_H

#include <awui/Windows/Forms/Button.h>

using namespace awui::Windows::Forms;

namespace awui {
	namespace Windows {
		namespace Emulators {
			class DebuggerSMS;
			class ArcadeContainer : public Button {
				public:
					virtual void SetSoundEnabled(bool mode) {}
					virtual void SetDebugger(DebuggerSMS * debugger) {};
					virtual int GetType() const = 0;
			};
		}
	}
}

#endif
