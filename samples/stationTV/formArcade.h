#pragma once

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				class StationUI;
			}
		}
	}
}

using namespace awui::Windows::Forms::Station;

class FormArcade : public awui::Windows::Forms::Form {
	private:
		StationUI * m_station;

		void InitializeComponent();

	public:
		FormArcade();
		virtual ~FormArcade();
};
