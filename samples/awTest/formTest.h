#pragma once

#include <awui/Windows/Forms/Form.h>
#include <awui/Random.h>

namespace awui::Windows::Forms {
	class Button;
}

using namespace awui::Windows::Forms;

class FormTest : public awui::Windows::Forms::Form {
	private:
		Button * m_buttonL;
		RemoteButtons::Enum m_buttonPressed;
		awui::Random m_rand;

		void InitializeComponent();

	public:
		FormTest();
		virtual ~FormTest() = default;

		virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button) override;
		virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button) override;
		virtual void OnTick(float deltaSeconds) override;
};
