#pragma once

#include <awui/Random.h>
#include <awui/UI/Form.h>

namespace awui::UI {
	class Button;
}

using namespace awui::UI;

class FormTest : public awui::UI::Form {
  private:
	Button *m_buttonL;
	Input::RemoteButtons::Enum m_buttonPressed;
	awui::Random m_rand;

	void InitializeComponent();

  public:
	FormTest();
	virtual ~FormTest() = default;

	virtual bool OnRemoteKeyPress(int which, Input::RemoteButtons::Enum button) override;
	virtual bool OnRemoteKeyUp(int which, Input::RemoteButtons::Enum button) override;
	virtual void OnTick(float deltaSeconds) override;
};
