#ifndef _FORMSPECTRUM_H
#define _FORMSPECTRUM_H

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

class FormSpectrum : public awui::Windows::Forms::Form {
	private:
		SliderBrowser * _slider;
		ArrayList * _games;
		DebuggerSMS * _debugger;

		void InitializeComponent();

	public:
		FormSpectrum();
		virtual ~FormSpectrum();

		void LoadRom(const awui::String file);

		virtual void OnTick();
};

#endif
