#pragma once

#include <awui/GOB/Engine/Renderers/Renderer.h>
#include <awui/String.h>
#include <awui/UI/Form.h>
#include <awui/UI/Input/Keys.h>

using namespace awui::GOB::Engine;
using namespace awui::UI;

class FormGOB : public awui::UI::Form {
  private:
	Renderer *m_renderer;

	void InitializeComponent();

  public:
	FormGOB();
	virtual ~FormGOB();

	bool OnKeyPress(Input::Keys::Enum key);
};
