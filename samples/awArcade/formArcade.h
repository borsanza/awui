#pragma once

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
			namespace Station {
				class StationUI;
			}

			class SliderBrowser;
		}
	}
}

using namespace awui::Collections;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Emulators;

class FormArcade : public awui::Windows::Forms::Form {
	private:
		StationUI * _station;

		void InitializeComponent();

	public:
		FormArcade();
		virtual ~FormArcade();
};
