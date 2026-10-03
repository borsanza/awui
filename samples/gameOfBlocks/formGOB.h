#pragma once

#include <awui/String.h>
#include <awui/UI/Form.h>
#include <awui/UI/Input/Keys.h>

#include <vector>

namespace awui::UI {
	class Label;
}

class World;

class FormGOB : public awui::UI::Form {
  private:
	World *m_world;
	std::vector<awui::UI::Label *> m_info; // Panel de información, abajo a la izquierda
	float m_infoSeconds;

	void InitializeComponent();
	void UpdateInfo();

  public:
	FormGOB();
	virtual ~FormGOB();

	virtual void OnTick(float deltaSeconds) override;
	virtual bool OnKeyPress(awui::UI::Input::Keys::Enum key) override;
	virtual bool OnKeyUp(awui::UI::Input::Keys::Enum key) override;
	virtual bool OnRemoteKeyUp(int which, awui::UI::Input::RemoteButtons::Enum button) override;
};
