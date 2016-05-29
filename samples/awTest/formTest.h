#pragma once

#include <awui/Windows/Forms/Form.h>
#include <awui/Random.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Button;
		}
	}
}

using namespace awui::Windows::Forms;

class FormTest : public awui::Windows::Forms::Form {
	private:
		Button * buttonL;
		RemoteButtons::Enum buttonPressed;
		awui::Random rand;

		void InitializeComponent();

	public:
		FormTest();
		virtual ~FormTest();

		virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
		virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
		virtual void OnTick();
};
