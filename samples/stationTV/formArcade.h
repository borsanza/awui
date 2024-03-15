#pragma once

#include <awui/String.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Keys.h>

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

	bool OnKeyPress(Keys::Enum key);
};
