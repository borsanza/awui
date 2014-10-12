#ifndef _FORMMASTERGEAR_H
#define _FORMMASTERGEAR_H

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Windows {
		namespace Emulators {
			class DebuggerSMS;
			class MasterSystem;
		}

		namespace Forms {
			class SliderBrowser;
		}
	}
}

using namespace awui::Collections;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

class FormMasterGear : public awui::Windows::Forms::Form {
	private:
		SliderBrowser * _slider;
		ArrayList * _games;
		DebuggerSMS * _debugger;

		void InitializeComponent();

	public:
		FormMasterGear();
		virtual ~FormMasterGear();

		void LoadRom(const awui::String file);
		
		virtual void OnTick();
};

#endif
