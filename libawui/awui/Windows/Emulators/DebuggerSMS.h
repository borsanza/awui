#ifndef _AWUI_WINDOWS_EMULATORS_DEBUGGERSMS_H
#define _AWUI_WINDOWS_EMULATORS_DEBUGGERSMS_H

#include <awui/Windows/Forms/Button.h>

using namespace awui::Windows::Forms;

namespace awui {
	namespace Windows {
		namespace Emulators {
			class MasterSystem;

			class DebuggerSMS : public Button {
				private:
					MasterSystem * _sms;

				public:
					DebuggerSMS(MasterSystem * sms);
					virtual ~DebuggerSMS();

					virtual int IsClass(Classes::Enum objectClass) const;

					virtual void OnTick();

					virtual void OnPaint(OpenGL::GL * gl);
			};
		}
	}
}

#endif