#pragma once

#include <awui/UI/Control.h>

namespace awui::UI {
	class ListBox;

	namespace Diagnostics {
		class Process;
	}
} // namespace awui::UI

class Test2 : public awui::UI::Control {
  private:
	bool m_runMame;
	bool m_endMame;
	awui::UI::ListBox *m_listbox;

	void InitializeComponent();

  public:
	Test2();
	virtual ~Test2() = default;

	virtual void OnTick(float deltaSeconds) override;

	void CheckMame();
	// void CheckGames();
};
