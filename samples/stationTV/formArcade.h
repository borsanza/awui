#pragma once

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Input/Keys.h>

namespace awui::Windows::Forms::Station {
	class StationUI;
}

using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Station;

class FormArcade : public awui::Windows::Forms::Form {
  private:
	StationUI *m_stationUI;

	void InitializeComponent();

  public:
	FormArcade();
	virtual ~FormArcade();

	bool OnKeyPress(Input::Keys::Enum key);
	virtual void OnClosing() override;
};
