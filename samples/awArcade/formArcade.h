#ifndef _FORMARCADE_H
#define _FORMARCADE_H

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Windows {
		namespace Emulators {
			class DebuggerSMS;
		}

		namespace Forms {
			class SliderBrowser;
		}
	}
}

using namespace awui::Collections;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Emulators;

class FormArcade : public awui::Windows::Forms::Form {
	private:
		SliderBrowser * _slider;
		ArrayList * _games;
		DebuggerSMS * _debugger;

		void InitializeComponent();

	public:
		FormArcade();
		virtual ~FormArcade();

		void LoadRom(const awui::String file);

		virtual void OnTick();
};

#endif
