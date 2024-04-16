#pragma once

#include <awui/GOB/Engine/Renderers/Renderer.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Keys.h>

using namespace awui::GOB::Engine;
using namespace awui::Windows::Forms;

class FormGOB : public awui::Windows::Forms::Form {
  private:
	Renderer *m_renderer;

	void InitializeComponent();

  public:
	FormGOB();
	virtual ~FormGOB();

	bool OnKeyPress(Keys::Enum key);
};
