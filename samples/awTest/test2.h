#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms {
	class ListBox;

	namespace Diagnostics {
		class Process;
	}
}

class Test2: public awui::Windows::Forms::Control {
	private:
		bool m_runMame;
		bool m_endMame;
		awui::Windows::Forms::ListBox *m_listbox;

		void InitializeComponent();

	public:
		Test2();
		virtual ~Test2() = default;

		virtual void OnTick(float deltaSeconds) override;

		void CheckMame();
		void CheckGames();
};
