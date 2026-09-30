#pragma once

#include <awui/String.h>
#include <awui/UI/Form.h>
#include <awui/UI/Input/Keys.h>

namespace awui::UI::Station {
	class StationUI;
}

using namespace awui::UI;
using namespace awui::UI::Station;

class FormArcade : public awui::UI::Form {
  private:
	StationUI *m_stationUI;

	void InitializeComponent();

  public:
	FormArcade();
	virtual ~FormArcade();

	bool OnKeyPress(Input::Keys::Enum key);
	virtual void OnClosing() override;
};
